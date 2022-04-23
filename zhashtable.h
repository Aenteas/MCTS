#ifndef ZHASHTABLE_H
#define ZHASHTABLE_H

#include "zhashtablebase.h"
#include <vector>
#include <array>
#include <memory>

using namespace std;

/**********************************************************************************
 * Zobrist HashTable implementation with simple replacement scheme                *
 * - Collision is handled by storing new items in arrays at each entry            *
 * - When the # of items to store would exceed 2 the following replacement scheme *
 * is employed:                                                *                  *
 * We discard the item that is deeper, if they are at the same depth we keep      *
 * the one that has been visited more                                             *
 **********************************************************************************/

template<typename T>
class ZHashTable: public ZHashTableBase<T>
{
    friend class Test;
public:
    ZHASHTABLEBASE_TYPEDEFS

    ZHashTable(unsigned moveNum, unsigned hashCodeSize=20);
    ~ZHashTable();

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

    vector<array<DHashNode*, 2>> table;
    DHashNode* root;
    DHashNode* rp;
};

template<typename T>
ZHashTable<T>::ZHashTable(unsigned moveNum, unsigned hashCodeSize):
    ZHashTableBase<T>(moveNum, hashCodeSize),
    depth(0),
    table{vector<array<DHashNode*, 2>>(pow(2, hashCodeSize), {nullptr, nullptr})},
    rp(new DHashNode())
{
    table[0][0] = new DHashNode();
    root = table[0][0];
}

template<typename T>
ZHashTable<T>::~ZHashTable()
{
    delete rp;
    for(auto& slot : table[Base::currCode]){
        for(auto p : slot){
            delete p;
        }
    }
    // root has no ownership so we do not need to delete it
}

template<typename T>
T* ZHashTable<T>::load(unsigned moveIdx)
{
    Base::update(moveIdx);
    ++depth;
    for(auto& slot : table[Base::currCode]){
        for(auto p : slot){
            if(p->impl.impl.key == Base::currKey)
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
                return addressof(p->impl.impl);
        }
    }
    return nullptr;
}

template<typename T>
template<class... Args>
T* ZHashTable<T>::store(Args&&... args)
{
    // replacement scheme
    // node deallocation is postponed after backpropagation
    // because node might be removed from the selection path
    if(!table[currCode][0]){
        table[currCode][0] = new T(Base::currKey, Base::currCode, Base::parent, depth, forward<Args>(args)...);
        return addressof(table[Base::currCode][0]->impl.impl);
    }
    else if(!table[currCode][1]){
        table[currCode][1] = new T(Base::currKey, Base::currCode, Base::parent, depth, forward<Args>(args)...);
        return addressof(table[Base::currCode][1]->impl.impl);
    }
    else{
        // reachable ? -> closer to root ? -> visit count ?
        // we overwrite replaced node after backpropagation
        if(table[Base::currCode][0]->depth <= root->depth){
            swap(table[Base::currCode][0], rp);
            table[Base::currCode][0]->reset(Base::currKey, Base::currCode, Base::parent, depth, forward<Args>(args)...)
            return addressof(table[Base::currCode][0]->impl.impl);
        }
        else if(table[Base::currCode][1]->depth <= root->depth){
            swap(table[Base::currCode][1], rp);
            table[Base::currCode][1]->reset(Base::currKey, Base::currCode, Base::parent, depth, forward<Args>(args)...)
            return addressof(table[Base::currCode][1]->impl.impl);
        }
        else if(table[Base::currCode][0]->depth > table[Base::currCode][1]->depth){
            swap(table[Base::currCode][0], rp);
            table[Base::currCode][0]->reset(Base::currKey, Base::currCode, Base::parent, depth, forward<Args>(args)...)
            return addressof(table[Base::currCode][0]->impl.impl);
        }
        else if(table[Base::currCode][0]->depth < table[Base::currCode][1]->depth){
            swap(table[Base::currCode][1], rp);
            table[Base::currCode][1]->reset(Base::currKey, Base::currCode, Base::parent, depth, forward<Args>(args)...)
            return addressof(table[Base::currCode][1]->impl.impl);
        }
        else if(table[Base::currCode][0]->visitCount() < table[Base::currCode][1]->visitCount()){
            swap(table[Base::currCode][0], rp);
            table[Base::currCode][0]->reset(Base::currKey, Base::currCode, Base::parent, depth, forward<Args>(args)...)
            return addressof(table[Base::currCode][0]->impl.impl);
        }
        else{
            swap(table[Base::currCode][1], rp);
            table[Base::currCode][1]->reset(Base::currKey, Base::currCode, Base::parent, depth, forward<Args>(args)...)
            return addressof(table[Base::currCode][1]->impl.impl);
        }
    }
}

template<typename T>
T* ZHashTable<T>::updateRoot(unsigned moveIdx){
    // load root
    Base::update(moveIdx);
    for(auto& slot : table[Base::currCode]){
        for(auto p : slot){
            if(p->impl.impl.key == Base::currKey){
                root = p;
                goto exit;
            }
        }
    }
    exit:
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
    return Base::parent;
}


#endif // ZHASHTABLE_H
