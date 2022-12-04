#ifndef ZHASHTABLE_H
#define ZHASHTABLE_H

#include "zhashtablebase.h"

#include <vector>
#include <array>
#include <memory>

/**********************************************************************************
 * Zobrist HashTable implementation with simple replacement scheme                *
 * - Collision is handled by storing new items in arrays at each entry            *
 * - When the # of items to store would exceed 2 the following replacement scheme *
 * is employed:                                                *                  *
 * We discard the item that is deeper, if they are at the same depth we keep      *
 * the one that has been visited more                                             *
 **********************************************************************************/

template<typename T>
class ZHashTable: public ZHashTableBase<ZHashTable<T>>
{
public:
    ZHASHTABLEBASE_TYPEDEFS(ZHashTable<T>)

    ZHashTable(unsigned moveNum, unsigned hashCodeSize=20);
    ~ZHashTable();

    ZHashTable(const ZHashTable&)=delete;
    ZHashTable& operator=(const ZHashTable&)=delete;
    ZHashTable(ZHashTable&&)=delete;
    ZHashTable& operator=(ZHashTable&&)=delete;
protected:
    // wrapper around HashNode storing the search depth
    struct DHashNode{
        // no forwarded arguments here: it is assumed that T has default arguments
        template<class... Args>
        DHashNode(ull key=0, ull code=0, T* parent=nullptr, unsigned depth=0, Args&&... args):
        impl(HashNode(key, code, parent, std::forward<Args>(args)...)),
        depth(depth){}

        template<class... Args>
        void reset(ull key, ull code, T* parent, unsigned depth, Args&&... args){
            this->depth = depth;
            impl.reset(key, code, parent, std::forward<Args>(args)...);
        }

        HashNode impl;
        unsigned depth;
    };
public:
    // loads node, returns nullptr when it is not in the table
    T* select(unsigned moveIdx) const;

    void update(unsigned moveIdx);

    template<class... Args>
    T* store(unsigned moveIdx, Args&&... args);
    // root needs to be overriden by the best child from the previous search
    template<class... Args>
    T* updateRoot(unsigned moveIdx, Args&&... args);
    // we can jump back to root after simulation as backpropagation is done inside node classes:
    // each node has a pointer to their parents so we do not need to search them in the table neither
    // we need to store the path in a stack
    T* selectRoot();
protected:
    std::vector<std::array<DHashNode*, 2>> table;
    DHashNode* root;
    DHashNode* rp;

    // current search depth
    unsigned depth;
};

template<typename T>
ZHashTable<T>::ZHashTable(unsigned moveNum, unsigned hashCodeSize):
    ZHashTableBase<ZHashTable<T>>(moveNum, hashCodeSize),
    depth(0),
    table{std::vector<std::array<DHashNode*, 2>>(pow(2, hashCodeSize), {nullptr, nullptr})},
    root(new DHashNode()),
    rp(new DHashNode())
{}

template<typename T>
ZHashTable<T>::~ZHashTable()
{
    delete rp;
    delete root;
    for(auto& slot : table){
        for(auto p : slot){
            if(p)
                delete p;
        }
    }
}

template<typename T>
T* ZHashTable<T>::select(unsigned moveIdx) const
{
    for(auto p : table[Base::currCode ^ Base::hashCodes[moveIdx]]){
        if(p && p->impl.key == (Base::currKey ^ Base::hashKeys[moveIdx])){
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

            return std::addressof(p->impl.impl);
        }
    }
    return nullptr;
}

template<typename T>
void ZHashTable<T>::update(unsigned moveIdx)
{
    Base::update(moveIdx);
    ++depth;
}

template<typename T>
template<class... Args>
T* ZHashTable<T>::store(unsigned moveIdx, Args&&... args)
{
    // zobrist hashing
    Base::currCode ^= Base::hashCodes[moveIdx];
    Base::currKey ^= Base::hashKeys[moveIdx];
    ++depth;
    // replacement scheme
    // node deallocation is postponed after backpropagation
    // because node might be removed from the selection path
    if(!table[Base::currCode][0]){
        table[Base::currCode][0] = new DHashNode(Base::currKey, Base::currCode, Base::parent, depth, std::forward<Args>(args)...);
        return std::addressof(table[Base::currCode][0]->impl.impl);
    }
    else if(!table[Base::currCode][1]){
        table[Base::currCode][1] = new DHashNode(Base::currKey, Base::currCode, Base::parent, depth, std::forward<Args>(args)...);
        return std::addressof(table[Base::currCode][1]->impl.impl);
    }
    else{
        // reachable ? -> closer to root ? -> visit count ?
        // we overwrite replaced node after backpropagation
        if(table[Base::currCode][0]->depth <= root->depth){
            std::swap(table[Base::currCode][0], rp);
            table[Base::currCode][0]->reset(Base::currKey, Base::currCode, Base::parent, depth, std::forward<Args>(args)...);
            return std::addressof(table[Base::currCode][0]->impl.impl);
        }
        else if(table[Base::currCode][1]->depth <= root->depth){
            std::swap(table[Base::currCode][1], rp);
            table[Base::currCode][1]->reset(Base::currKey, Base::currCode, Base::parent, depth, std::forward<Args>(args)...);
            return std::addressof(table[Base::currCode][1]->impl.impl);
        }
        else if(table[Base::currCode][0]->depth > table[Base::currCode][1]->depth){
            std::swap(table[Base::currCode][0], rp);
            table[Base::currCode][0]->reset(Base::currKey, Base::currCode, Base::parent, depth, std::forward<Args>(args)...);
            return std::addressof(table[Base::currCode][0]->impl.impl);
        }
        else if(table[Base::currCode][0]->depth < table[Base::currCode][1]->depth){
            std::swap(table[Base::currCode][1], rp);
            table[Base::currCode][1]->reset(Base::currKey, Base::currCode, Base::parent, depth, std::forward<Args>(args)...);
            return std::addressof(table[Base::currCode][1]->impl.impl);
        }
        else if(table[Base::currCode][0]->impl.impl.getVisitCount() < table[Base::currCode][1]->impl.impl.getVisitCount()){
            std::swap(table[Base::currCode][0], rp);
            table[Base::currCode][0]->reset(Base::currKey, Base::currCode, Base::parent, depth, std::forward<Args>(args)...);
            return std::addressof(table[Base::currCode][0]->impl.impl);
        }
        else{
            std::swap(table[Base::currCode][1], rp);
            table[Base::currCode][1]->reset(Base::currKey, Base::currCode, Base::parent, depth, std::forward<Args>(args)...);
            return std::addressof(table[Base::currCode][1]->impl.impl);
        }
    }
}

template<typename T>
template<class... Args>
T* ZHashTable<T>::updateRoot(unsigned moveIdx, Args&&... args){
    // zobrist hashing
    Base::currCode ^= Base::hashCodes[moveIdx];
    Base::currKey ^= Base::hashKeys[moveIdx];
    ++depth;
    for(auto& p : table[Base::currCode]){
        if(p && p->impl.key == Base::currKey){
            std::swap(root, p);
            root->impl.impl.parent = nullptr;
            goto found;
        }
    }
    root->reset(Base::currKey, Base::currCode, nullptr, depth, std::forward<Args>(args)...);
    found:
    Base::parent = std::addressof(root->impl.impl);
    return Base::parent;
}

template<typename T>
T* ZHashTable<T>::selectRoot()
{
    // update
    depth = root->depth;
    Base::currKey = root->impl.key;
    Base::currCode = root->impl.code;
    Base::parent = std::addressof(root->impl.impl);
    return Base::parent;
}

#endif // ZHASHTABLE_H
