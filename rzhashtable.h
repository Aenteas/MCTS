#ifndef RZHASHTABLE_H
#define RZHASHTABLE_H

#include <iostream>
#include <algorithm>
#include <list>
#include <memory>

#include "zhashtablebase.h"
using namespace std;

/**********************************************************************************
 * Recycling Zobrist HashTable                                                    *
 * - When the number of nodes exceeds the budget, it discards the least recently  *
 * visited leaf node from the search tree.                                        *
 * - Employs open-addressing with linear probing which has better performance     *
 * than chained hashtables when the load factor is low                            *
 * (budget << # of hash entries)                                                  *
 * - Since the # of lookups are a lot higher (~average search depth times)        *
 * than the # of insertions/deletions during a Monte Carlo tree search, we        *
 * immetiately fill up deleted entries so the probability of finding an item      *
 * right away is the maximum (~ 1-1/(load factor))                                *
 * - There is no heap allocation during the search phase                          *
 **********************************************************************************/

template<typename T>
class RZHashTable: public ZHashTableBase<T>
{
    friend class Test;
public:
    ZHASHTABLEBASE_TYPEDEFS

    RZHashTable(unsigned moveNum, unsigned hashCodeSize=20, unsigned budget=50000);
    ~RZHashTable()=default;

    RZHashTable(const RZHashTable&)=delete;
    RZHashTable& operator=(const RZHashTable&)=delete;
    RZHashTable(const RZHashTable&&)=delete;
    RZHashTable& operator=(const RZHashTable&&)=delete;

    // loads node, returns nullptr when it is not in the table
    T* load(unsigned moveIdx);
    template<class... Args>
    T* store(Args&&... args);
    // root needs to be overriden by the best child from the previous search
    T* updateRoot(unsigned moveIdx);
    // we can jump back to root after simulation as backpropagation is done inside node classes:
    // each node has a pointer to their parents so we do not need to search them in the table neither
    // we need to store the path in a stack
    T* selectRoot();
protected:
    // the last 2 table entries (are not addressable) are stored as dummy entries so we can use them as flags
    // and to initialize the table so no valid entries are overridden in store function at the begining
    // this way we can spare an if statement in the store function
    // node is empty when the node code is the index for the last entry
    bool isEmpty(const typename list<HashNode>::iterator& it) const;
    void setEmpty(typename list<HashNode>::iterator& it);

    // empty code
    const ull EMPTYCODE;
    // empty node
    list<HashNode> empty;

    unsigned budget;
    // least recently visited node to discard is at the beginning
    // technically not a fifo because we need to move interior nodes to the end each time they are visited
    list<HashNode> fifo;
    // maps hash values to iterators in the fifo
    vector<typename list<HashNode>::iterator> table;
    // we update the fifo during the selection phase (visited ones should go to the back)
    // in the selection phase nodes need to be inserted before their parents and target stores that location
    // it would be more clean to do it during backpropagation (so nodes just can be pushed to the back)
    // but this way the caller do not need to rely on a stack storing the moves/nodes
    // and we can also just jump back to root after simulation
    typename list<HashNode>::iterator target;
    // code stores the result (hash value) from the last linear probing
    // so later we can store the new node at the proper location
    ull code;
};

template<typename T>
RZHashTable<T>::RZHashTable(unsigned moveNum, unsigned hashCodeSize, unsigned budget):
    ZHashTableBase<T>(moveNum, hashCodeSize),
    EMPTYCODE(pow(2, hashCodeSize)),
    empty(1, HashNode(0,EMPTYCODE)),
    budget(budget),
    fifo(budget, HashNode(0,EMPTYCODE)), // preallocate nodes
    code(0)
{
    // +2 additional dummy entries at the end
    unsigned tableSize = pow(2, hashCodeSize) + 2;
    //if(tableSize - 2 < 2 * budget)
    //    throw std::invalid_argument( "RZHashTable: load factor should not exceed 0.5" );

    // last is root
    fifo.back().code = 0;
    // next insertion before root
    target = fifo.end();
    --target;

    table = vector<typename list<HashNode>::iterator>(tableSize, empty.begin());
    table[Base::currCode] = target; // set root in table
}

template<typename T>
inline bool RZHashTable<T>::isEmpty(const typename list<HashNode>::iterator& it) const{
    return it == empty.begin();
}

template<typename T>
inline void RZHashTable<T>::setEmpty(typename list<HashNode>::iterator& it) {
    it = empty.begin();
}

template<typename T>
T* RZHashTable<T>::load(unsigned moveIdx)
{
    Base::update(moveIdx);
    code = Base::currCode;
    auto it = table[code];
    // linear probing
    // there is no infinite loop as the number of
    // nodes are strictly smaller than the number of entries
    while(!isEmpty(it)) {
        // node is in the table
        if(it->key == Base::currKey){
            /**
               There is almost zero probability that 2 states from the current selection path will be mapped
               to the same entry with the same hashKey. But when it happens we
               have an infinite loop. We could check that with additional computation and storage but the problem is
               that loops might be naturally part of the searching tree (like in chess). It is not really
               the hashtable's job to handle that so we leave it to the searching algorithm. Worst case if it is not
               handled the search should run out of time and the thread should be interrupted in which case the currently best
               move is returned.

               In the other case, when a newly discovered state is mapped to an other one that is not in the current selection path
               we potentially update the nodes with the wrong statistics but this effect is negligible compared to the cost of
               handling it.
            **/

            // update position in fifo
            fifo.splice(target, fifo, it);
            target = it;
            // in case Ts' & operator is overloaded
            // * operator gives reference
            it->impl.parent = Base::parent;
            // set parent so it can be provided to the next children
            Base::parent = addressof(it->impl);
            return Base::parent;
        }
        ++code;
        it = table[code];
    }
    // node is not in the table
    return nullptr;
}

template<typename T>
template<class... Args>
T* RZHashTable<T>::store(Args&&... args)
{
    // get hash value of node to remove
    ull targetCode, sourceCode;
    targetCode = fifo.front().code;
    // find exact location
    while(table[targetCode]->key != fifo.front().key)
        ++targetCode;
    sourceCode = targetCode + 1;

    // we insert before shifting so we do not need to check if we need to shift it afterwards
    // override the least recently visited leaf node by the new one
    fifo.front().reset(Base::currKey, Base::currCode, Base::parent, forward<Args>(args)...);
    // set the iterator in the hash table
    auto it = fifo.begin();
    table[code] = it;

    // fill up empty place with shifted entries
    // this way we do not need to mark nodes as deleted (as you would need with open-addressing)
    // and we can reduce the lookup time
    // this is preferable as we have a lot more lookups than insertions/removal
    while(!isEmpty(table[sourceCode])) {
        // check if we can shift from source (that is hashcode is between the target and its current location)
        // normal in-between comparison would not work because of overflow
        if(sourceCode < targetCode ?
           (table[sourceCode]->code - sourceCode - 1 <= targetCode - sourceCode - 1) :
           (table[sourceCode]->code - targetCode - 1 >= sourceCode - targetCode))
        {
            table[targetCode] = table[sourceCode];
            targetCode = sourceCode;
        }
        ++sourceCode;
    }

    // set last source entry to empty to remove duplication or the first one if there was no shift
    setEmpty(table[targetCode]);
    // update position in fifo
    fifo.splice(target, fifo, it);
    // after leaf the next node to visit is root
    target = fifo.end();
    --target;
    return addressof(it->impl);
}

template<typename T>
T* RZHashTable<T>::updateRoot(unsigned moveIdx){
    Base::update(moveIdx);
    code = Base::currCode;
    auto it = table[code];
    while(!isEmpty(it) && it->key != Base::currKey) {
        ++code;
        it = table[code];
    }

    // move old root to the beginning of fifo to be overridden first
    auto root = fifo.end();
    --root;
    fifo.splice(fifo.begin(), fifo, root);

    // move new root to the last position
    fifo.splice(fifo.end(), fifo, it);
    target = it;
    it->impl.parent = nullptr;
    // set parent so it can be provided to the next children
    Base::parent = addressof(it->impl);
    return Base::parent;
}

template<typename T>
T* RZHashTable<T>::selectRoot()
{
    // root is at the end of fifo
    auto root = fifo.end();
    --root;
    // update
    Base::currKey = root->key;
    Base::currCode = root->code;
    Base::parent = addressof(root->impl);
    return Base::parent;
}

#endif // RZHASHTABLE_H
