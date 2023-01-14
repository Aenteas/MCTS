#ifndef RZHASHTABLE_H
#define RZHASHTABLE_H

#include "zhashtablebase.h"
#include "parallellist.h"

#include <algorithm>
#include <list>
#include <memory>

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
class RZHashTable: public ZHashTableBase<RZHashTable<T>>
{
public:
    ZHASHTABLEBASE_SETUP(RZHashTable<T>)

    RZHashTable(unsigned moveNum, unsigned maxDepth, unsigned hashCodeSize=20, unsigned budget=50000);
    ~RZHashTable()=default;

    RZHashTable(const RZHashTable&)=delete;
    RZHashTable& operator=(const RZHashTable&)=delete;
    RZHashTable(RZHashTable&&)=delete;
    RZHashTable& operator=(RZHashTable&&)=delete;

    // loads node, returns nullptr when it is not in the table
    T* select(unsigned moveIdx);
    template<class... Args>
    T* store(unsigned moveIdx, Args&&... args);
    // root needs to be overriden by the best child from the previous search
    template<class... Args>
    T* updateRoot(unsigned moveIdx, Args&&... args);

    void update(unsigned moveIdx);

protected:
    // the last 2 table entries (are not addressable) are stored as dummy entries so we can use them as flags
    // and to initialize the table so no valid entries are overridden in store function at the begining
    // this way we can spare an if statement in the store function
    // node is empty when the node code is the index for the last entry
    bool isEmpty(typename ParallelList<HashNode>::Node* p) const;
    void setEmpty(typename ParallelList<HashNode>::Node* p);

    void setupExploration();

    // empty code
    const ull EMPTYCODE;
    // empty node
    ParallelList<HashNode> empty;

    // least recently visited node to discard is at the beginning
    // technically not a fifo because we need to move interior nodes to the end each time they are visited
    ParallelList<HashNode> fifo;
    // maps hash values to iterators in the fifo
    std::vector<typename ParallelList<HashNode>::Node*> table;
    // we update the fifo during the selection phase (visited ones should go to the back)
    // in the selection phase nodes need to be inserted before their parents and target stores that location
    // alternatively we could do it during backpropagation (so nodes just can be pushed to the back)
    // but this way the caller do not need to rely on a stack storing the moves/nodes
    // and the fifo member can be better parallelized
    typename ParallelList<HashNode>::Node* target;

    // code stores the result (hash value) from the last linear probing
    // so later we can store the new node at the proper location
    ull code;
};

template<typename T>
RZHashTable<T>::RZHashTable(unsigned moveNum, unsigned maxDepth, unsigned hashCodeSize, unsigned budget):
    ZHashTableBase<RZHashTable<T>>(moveNum, maxDepth, hashCodeSize),
    EMPTYCODE(pow(2, hashCodeSize)),
    empty(ParallelList<HashNode>(2, 0, EMPTYCODE)),
    fifo(ParallelList<HashNode>(budget, 0, EMPTYCODE)), // preallocate nodes
    code(0)
{
    unsigned tableSize = pow(2, hashCodeSize);
    if(tableSize < 2 * budget)
        throw std::invalid_argument( "RZHashTable: load factor should not exceed 0.5" );
    if(budget < maxDepth + 1)
        throw std::invalid_argument( "RZHashTable: budget should be greater than " + std::to_string(maxDepth) );
    // last is root
    fifo.back()->data.code = 0;
    // next insertion before root
    target = fifo.back();

    // +2 additional dummy entries at the end
    table = std::vector<typename ParallelList<HashNode>::Node*>(tableSize + 2, empty._front);
    table[Base::currCode] = target; // set root in table
}

template<typename T>
inline bool RZHashTable<T>::isEmpty(typename ParallelList<HashNode>::Node* p) const{
    return p == empty._front;
}

template<typename T>
inline void RZHashTable<T>::setEmpty(typename ParallelList<HashNode>::Node* p) {
    p = empty._front;
}

template<typename T>
T* RZHashTable<T>::select(unsigned moveIdx)
{
    code = Base::currCode ^ Base::hashCodes[moveIdx];
    auto p = table[code];
    // linear probing
    // there is no infinite loop as the number of
    // nodes are strictly smaller than the number of entries
    while(!isEmpty(p)) {
        // node is in the table
        if(p->data.key == (Base::currKey ^ Base::hashKeys[moveIdx])){
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

            // in case Ts' & operator is overloaded
            // * operator gives reference
            return std::addressof(p->data.impl);
        }
        code = (code + 1) & Base::hashCodeMask;
        p = table[code];
    }
    // node is not in the table
    return nullptr;
}

template<typename T>
void RZHashTable<T>::update(unsigned moveIdx)
{
    // Zobrist hashing
    Base::currCode ^= Base::hashCodes[moveIdx];
    Base::currKey ^= Base::hashKeys[moveIdx];
    code = Base::currCode;
    auto p = table[code];
    while(!isEmpty(p)) {
        // node is in the table
        if(p->data.key == Base::currKey){

            // in case Ts' & operator is overloaded
            // * operator gives reference
            // update position in fifo
            target = fifo.splice(p, target);
            // add new node to selection path
            Base::path[Base::depth] = std::addressof(p->data);
            ++Base::depth;
            return;
        }
        code = (code + 1) & Base::hashCodeMask;
        p = table[code];
    }
}

template<typename T>
template<class... Args>
T* RZHashTable<T>::store(unsigned moveIdx, Args&&... args)
{
    // update code
    T* node = select(moveIdx);
    // Zobrist hashing
    Base::currCode ^= Base::hashCodes[moveIdx];
    Base::currKey ^= Base::hashKeys[moveIdx];
    ++Base::depth;

    if(node)
        return node;

    // update position in fifo: move front before current parent
    auto p = fifo.spliceFront(target);

    // add new node to selection path
    Base::path[Base::depth - 1] = std::addressof(p->data);

    // get location of node to remove
    ull targetCode, sourceCode;
    targetCode = p->data.code;

    // to restore dummy address, only bit manipulation, no if is needed
    ull ov = targetCode & ~Base::hashCodeMask;
    // find exact location
    while(table[targetCode]->data.key != p->data.key)
        targetCode = (targetCode + 1) & Base::hashCodeMask;
    sourceCode = (targetCode + 1) & Base::hashCodeMask | ov;

    // we insert before shifting so we do not need to check if we need to shift it afterwards
    // override the least recently visited leaf node by the new one
    p->data.reset(Base::currKey, Base::currCode, std::forward<Args>(args)...);
    // set the iterator in the hash table
    table[code] = p;

    // ---- remove least recently visited leaf from hashtable ----

    // fill up empty place with shifted entries
    // this way we do not need to mark nodes as deleted (as you would need with open-addressing)
    // and we can reduce the lookup time
    // this is preferable as we have a lot more lookups than insertions/removal
    while(!isEmpty(table[sourceCode])) {
        // check if we can shift from source (that is hashcode is between the target and its current location)
        // normal in-between comparison would not work because of overflow
        if(sourceCode < targetCode ?
           (table[sourceCode]->data.code <= targetCode && table[sourceCode]->data.code > sourceCode) :
           (table[sourceCode]->data.code <= targetCode || table[sourceCode]->data.code > sourceCode))
        {
            table[targetCode] = table[sourceCode];
            targetCode = sourceCode;
        }
        sourceCode = (sourceCode + 1) & Base::hashCodeMask;
    }
    // set last source entry to empty to remove duplication or the first one if there was no shift
    setEmpty(table[targetCode]);
    return std::addressof(p->data.impl);
}

template<typename T>
template<class... Args>
T* RZHashTable<T>::updateRoot(unsigned moveIdx, Args&&... args){
    // no synchronization is needed, function is not used concurrently
    code = Base::currCode ^ Base::hashCodes[moveIdx];
    auto p = table[code];
    while(!isEmpty(p) && p->data.key != (Base::currKey ^ Base::hashKeys[moveIdx])) {
        code = (code + 1) & Base::hashCodeMask;
        p = table[code];
    }

    // move old root to the beginning of fifo to be overridden
    fifo.spliceRoot();
    ++Base::rootDepth;
    // if new root is not in table
    if(isEmpty(p)){
        target = fifo.end(); // root will be the last in fifo
        T* root = store(moveIdx, std::forward<Args>(args)...);
        // selected node will be moved in front of the root in the FIFO
        target = fifo.back();
        return root;
    }
    else{
        // Zobrist hashing
        Base::currCode ^= Base::hashCodes[moveIdx];
        Base::currKey ^= Base::hashKeys[moveIdx];
        Base::path[Base::depth] = std::addressof(p->data);
        ++Base::depth;
        // move new root to the last position
        target = fifo.splice(p, fifo.end());
        return std::addressof(p->data.impl);
    }
}

template<typename T>
void RZHashTable<T>::setupExploration(){
    target = fifo.back();
}

#endif // RZHASHTABLE_H
