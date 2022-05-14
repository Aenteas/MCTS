#ifndef ZHASHTABLEBASE_H
#define ZHASHTABLEBASE_H

#include <vector>
#include <random>
#include <math.h>

using namespace std;

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
    ZHashTableBase(unsigned moveNum, unsigned hashCodeSize=20);
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
        HashNode(ull key=0, ull code=0, typename nodeType<T>::value_type* parent=nullptr, Args&&... args):
        impl(parent, forward<Args>(args)...),
        key(key),
        code(code){}

        template<class... Args>
        void reset(ull key, ull code, typename nodeType<T>::value_type* parent, Args&&... args){
            this->key = key;
            this->code = code;
            impl.reset(parent, forward<Args>(args)...);
        }
        typename nodeType<T>::value_type impl;
        ull key;
        ull code;
    };
public:
    // do not call this function on leaf node, use expand instead!
    void update(unsigned moveIdx);
    typename nodeType<T>::value_type* selectCurrent();
protected:
    typename nodeType<T>::value_type* parent;
    // hashcode to map table entries
    vector<ull> hashCodes;
    // unique node identifiers
    vector<ull> hashKeys;
    // the hashcode of the current gamestate (node)
    ull currCode;
    ull hashCodeMask;
    // the hashkey of the current gamestate (node)
    ull currKey;
};

template<typename T>
ZHashTableBase<T>::ZHashTableBase(unsigned moveNum, unsigned hashCodeSize):
    parent(nullptr),
    currCode(0),
    currKey(0)
{
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

        n = distr(eng);
        while(any_of(hashKeys.begin(), hashKeys.begin() + i, [=](ull item){ return item == n; }))
        { n = distr(eng); }
        hashKeys.push_back(n);
    }
}

template<typename T>
void ZHashTableBase<T>::update(unsigned moveIdx)
{
    // static cast is safe as only derived class T can instantiate ZHashTableBase<T>
    typename nodeType<T>::value_type* child = static_cast<T&>(*this).select(moveIdx);
    // Zobrist hashing
    currCode ^= hashCodes[moveIdx];
    currKey ^= hashKeys[moveIdx];
    // no null pointer check, do not call this function on leaf node!
    child->parent = parent;
    // set parent so it can be provided to the next children
    parent = child;
}

template<typename T>
typename nodeType<T>::value_type* ZHashTableBase<T>::selectCurrent()
{
    // select could be specified for each derived class if we need to remove additional XOR operations
    // Dummy Zobrist hashing
    currCode ^= hashCodes[0];
    currKey ^= hashKeys[0];
    // static cast is safe as only derived class T can instantiate ZHashTableBase<T>
    typename nodeType<T>::value_type* child = static_cast<T&>(*this).select(0);
    // Dummy Zobrist hashing
    currCode ^= hashCodes[0];
    currKey ^= hashKeys[0];
    return child;
}

// ZHashTableBase<T> class is dependent so scope is not examined in derived classes during unqualified name lookup
// helper macro to use in derived classes
#define ZHASHTABLEBASE_TYPEDEFS(type) typedef ZHashTableBase<type> Base; \
                                typedef typename ZHashTableBase<type>::HashNode HashNode; \
                                typedef typename ZHashTableBase<type>::ull ull; \

#endif // ZHASHTABLEBASE_H
