#ifndef ZHASHTABLE_H
#define ZHASHTABLE_H

#include "zhashtablebase.h"
#include <vector>
#include <list>
#include <memory>

using namespace std;

/**********************************************************************************
 * Zobrist HashTable implementation with simple replacement scheme                *
 * - Collision is handled by storing new items in lists at each entry (chaining)  *
 * - When the size of the list exceeds a limit (chainSize) the following          *
 * replacement scheme is employed:                                                *
 * We discard the item that is deeper, if they are at the same depth we keep      *
 * the one that has been visited more                                             *
 *                                                                                *
 * Additional requirements on node type T:                                        *
 * - defined copy and moving assignment operator                                  *
 **********************************************************************************/

template<typename T>
class ZHashTable: public ZHashTableBase<T>
{
    friend class Test;
public:
    ZHASHTABLEBASE_TYPEDEFS

    ZHashTable(unsigned moveNum, unsigned hashCodeSize=20, unsigned chainSize=2);
    ~ZHashTable() { delete root; }

    ZHashTable(const ZHashTable&)=delete;
    ZHashTable& operator=(const ZHashTable&)=delete;
    ZHashTable(const ZHashTable&&)=delete;
    ZHashTable& operator=(const ZHashTable&&)=delete;
protected:
    // wrapper around HashNode storing the search depth
    struct DHashNode{
        // no forwarded arguments here: it is assumed that T has default arguments
        template<class... Args>
        DHashNode(ull key=0, ull code=0, T* parent=nullptr, unsigned depth=0, Args&&... args):
        impl{HashNode(key, code, parent, forward<Args>(args)...)},
        depth(depth){}

        HashNode impl;
        unsigned depth;
    };
public:
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
    // current search depth
    unsigned depth;

    // maximum length of the list at each entry
    unsigned chainSize;

    vector<list<DHashNode>> table;
    DHashNode* root;

    // entry index and list iterator to be removed. Node removal is postponed after backpropagation
    ull rCode;
    typename list<DHashNode>::iterator rIt;
};

template<typename T>
ZHashTable<T>::ZHashTable(unsigned moveNum, unsigned hashCodeSize, unsigned chainSize):
    ZHashTableBase<T>(moveNum, hashCodeSize),
    depth(0),
    chainSize(chainSize),
    table{vector<list<DHashNode>>(pow(2, hashCodeSize), list<DHashNode>())},
    root(new DHashNode()),
    rCode(pow(2, hashCodeSize)) // invalid value so we can check it when there is no collision so far
{}

template<typename T>
T* ZHashTable<T>::load(unsigned moveIdx)
{
    Base::update(moveIdx);
    for(auto& item : table[Base::currCode]){
        if(item.impl.impl.key == Base::currKey)
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
            return addressof(item.impl.impl);
    }
    ++depth;
    return nullptr;
}

template<typename T>
template<class... Args>
T* ZHashTable<T>::store(Args&&... args)
{
    // replacement scheme
    // node deallocation is postponed after backpropagation
    // because node might be removed from the selection path
    if(table[currCode].size() == 2){
        // reachable ? -> closer to root ? -> visit count ?
        // node deallocation is postponed after backpropagation
        if(table[Base::currCode].front().depth <= root->depth){
            rIt = table[Base::currCode].begin();
        }
        else if(table[Base::currCode].back().depth <= root->depth){
            rIt = table[Base::currCode].end();
            --rIt;
        }
        else if(table[Base::currCode].front().depth > table[Base::currCode].back().depth){
            rIt = table[Base::currCode].begin();
        }
        else if(table[Base::currCode].front().depth < table[Base::currCode].back().depth){
            rIt = table[Base::currCode].end();
            --rIt;
        }
        else if(table[Base::currCode].front().visitCount() < table[Base::currCode].back().visitCount()){
            rIt = table[Base::currCode].begin();
        }
        else{
            rIt = table[Base::currCode].end();
            --rIt;
        }
        rCode = Base::currCode;
    }
    table[Base::currCode].emplace_front(Base::currKey, Base::currCode, Base::parent, depth, forward<Args>(args)...);
    return addressof(*table[Base::currCode].begin());
}

template<typename T>
T* ZHashTable<T>::updateRoot(unsigned moveIdx){
    // load root
    Base::update(moveIdx);
    auto it = table[Base::currCode].begin();
    while(it != table[Base::currCode].end()){
        if(item.impl.impl.key == Base::currKey){
            root = addressof(move(*it));
            // remove from table
            table[Base::currCode].erase(it);
            break;
        }
        ++it;
    }
    ++depth;
    root->impl.impl.parent = nullptr;
    Base::parent = addressof(root->impl.impl);
    return Base::parent;
}

template<typename T>
T* ZHashTable<T>::selectRoot()
{
    // update
    depth = root->depth;
    Base::currKey = root->impl.key;
    Base::currCode = root->impl.code;
    Base::parent = addressof(root->impl.impl);
    // remove replaced node if there was a collision
    if(rCode < table.size()){
        table[rCode].erase(rIt);
        rCode = table.size();
    }
    return Base::parent;
}


#endif // ZHASHTABLE_H
