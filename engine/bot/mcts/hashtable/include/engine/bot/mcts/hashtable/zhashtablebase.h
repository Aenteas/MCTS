#ifndef ZHASHTABLEBASE_H
#define ZHASHTABLEBASE_H

#include <vector>
#include <random>
#include <math.h>

/**********************************************************************************
 * Extending hashtable classes with zobrist hashing using CRTP design pattern     *
 *                                                                                *
 * Requirements on node type T:                                                   *
 * - default constructor for creating root node                                   *
 * - storing a pointer to a parent node                                           *
 * - reset function to override node                                              *
 **********************************************************************************/

// type_trait to get node type
template<typename T>
struct nodeType
{
    typedef T value_type;
};

template<template<typename> class T, typename N>
struct nodeType<T<N>>   //specialization
{
    typedef N value_type;
};

template<typename T>
class ZHashTableBase
{
    // this ensures T to be derived from ZHashTableBase and prevents wrong usage that could cause undefined behavior (static cast in update function)
    friend T;
private:
    ZHashTableBase(unsigned moveNum, unsigned maxDepth, unsigned hashCodeSize=20);
protected:
    typedef unsigned long long ull;
    ~ZHashTableBase()=default;

    ZHashTableBase(const ZHashTableBase&)=delete;
    ZHashTableBase& operator=(const ZHashTableBase&)=delete;
    ZHashTableBase(ZHashTableBase&&)=delete;
    ZHashTableBase& operator=(ZHashTableBase&&)=delete;

    // wrapper around underlying node type
    struct HashNode{
        template<class... Args>
        HashNode(ull key=0, ull code=0, Args&&... args):
        impl(std::forward<Args>(args)...),
        key(key),
        code(code){}

        template<class... Args>
        void reset(ull key, ull code, Args&&... args){
            this->key = key;
            this->code = code;
            impl.reset(std::forward<Args>(args)...);
        }
        typename nodeType<T>::value_type impl;
        ull key;
        ull code;
    };
public:
    typename nodeType<T>::value_type* backward();

    template<class... Args>
    typename nodeType<T>::value_type* createRoot(Args&&... args);

private:
    // hashcode to map table entries
    inline static std::vector<ull> hashCodes;
    // unique node identifiers
    inline static std::vector<ull> hashKeys;
    // the hashcode of the current gamestate (node)
    ull currCode;
    // the hashkey of the current gamestate (node)
    ull currKey;
    // currently selected hashkeys stored for backpropagation
    unsigned depth;
    inline static unsigned rootDepth;
    std::vector<HashNode*> path;
    inline static ull hashCodeMask;
};

template<typename T>
ZHashTableBase<T>::ZHashTableBase(unsigned moveNum, unsigned maxDepth, unsigned hashCodeSize):
    currCode(0),
    currKey(0),
    depth(0)
{
    rootDepth = 0;
    // this is unlikely but we check it for completeness
    if(moveNum > pow(2, hashCodeSize))
        throw std::invalid_argument( "RZHashTable: number of possible moves is greater than the number of entries" );
    // pick moveNum hashCodes randomly without repetition
    // normally you would use sample or random_shuffle but not for 64 bit
    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<ull> distr;
    // since the number of entries is multiplies of 2 we can unset the most signifficant bits
    // this way indices map to the table correctly and we do not need to apply the
    // modulo operator for each lookup
    hashCodeMask=(1L<<hashCodeSize)-1;

    hashCodes.reserve(moveNum);
    hashKeys.reserve(moveNum);
    
    for(unsigned i = 0;i<moveNum;++i){
        ull n = distr(eng) & hashCodeMask;
        while(any_of(hashCodes.begin(), hashCodes.begin() + i, [=](ull item){ return item == n; }))
        { n = distr(eng) & hashCodeMask; }
        hashCodes.push_back(n);

        // prevent parent-child and sibling nodes to have the same hashkeys. Good for concurrency optimization
        n = distr(eng);
        while(any_of(hashKeys.begin(), hashKeys.begin() + i, [=](ull item){ return item == n || n == 0; }))
        { n = distr(eng); }
        hashKeys.push_back(n);
    }
    path = std::vector<HashNode*>(maxDepth + 1, nullptr);
}

#include <iostream>
template<typename T>
typename nodeType<T>::value_type* ZHashTableBase<T>::backward()
{
    if(rootDepth < depth){
        --depth;
        auto parent = path[depth];
        if(!parent)
            std::cout << "ouch" << std::endl; 
        currCode = path[depth]->code;
        currKey = path[depth]->key;
        return std::addressof(parent->impl);
    }
    else{
        static_cast<T&>(*this).setupExploration();
        return nullptr;
    }
}
template<typename T>
template<class... Args>
typename nodeType<T>::value_type* ZHashTableBase<T>::createRoot(Args&&... args)
{
    currCode ^= hashCodes[0];
    currKey ^= hashKeys[0];
    auto root = static_cast<T&>(*this).store(0, std::forward<Args>(args)...);
    --depth;
    static_cast<T&>(*this).setupExploration();
    return root;
}


// ZHashTableBase<T> class is dependent so scope is not examined in derived classes during unqualified name lookup
// helper macro to use in derived classes
#define ZHASHTABLEBASE_SETUP(type) typedef ZHashTableBase<type> Base; \
                                typedef typename ZHashTableBase<type>::HashNode HashNode; \
                                typedef typename ZHashTableBase<type>::ull ull; \
                                friend class ZHashTableBase<type>; \

#endif // ZHASHTABLEBASE_H
