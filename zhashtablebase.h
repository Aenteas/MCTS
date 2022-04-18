#ifndef ZHASHTABLEBASE_H
#define ZHASHTABLEBASE_H

#include <vector>
#include <random>
#include <math.h>

using namespace std;

/**********************************************************************************
 * Base class for hashtables using Zobrist Hashing                                *
 *                                                                                *
 * Requirements on node type T:                                                   *
 * - default constructor for creating root node                                   *
 * - storing a pointer to a parent node                                           *
 * - reset function to override node                                              *
 **********************************************************************************/

template<typename T>
class ZHashTableBase
{
    friend class Test;
protected:
    typedef unsigned long long ull;

    ZHashTableBase(unsigned moveNum, unsigned hashCodeSize=20);
    ~ZHashTableBase()=default;

    ZHashTableBase(const ZHashTableBase&)=delete;
    ZHashTableBase& operator=(const ZHashTableBase&)=delete;
    ZHashTableBase(const ZHashTableBase&&)=delete;
    ZHashTableBase& operator=(const ZHashTableBase&&)=delete;

    // wrapper around underlying node type
    struct HashNode{
        template<class... Args>
        HashNode(ull key=0, ull code=0, T* parent=nullptr, Args&&... args):
        // impl{T(parent, forward<Args>(args)...)},
        impl(key, code, parent, forward<Args>(args)...),
        key(key),
        code(code){}

        template<class... Args>
        void reset(ull key, ull code, T* parent, Args&&... args){
            this->key = key;
            this->code = code;
            // impl.reset(parent, forward<Args>(args)...);
            impl.reset(key, code, parent, forward<Args>(args)...);
        }
        T impl;
        ull key;
        ull code;
    };
public:
    void update(unsigned moveIdx);
    // update is only called directly from the outside when we are moving back the previous state
    void backward(unsigned moveIdx);
protected:
    T* parent;

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
    //if(moveNum > pow(2, hashCodeSize))
    //    throw std::invalid_argument( "RZHashTable: number of possible moves is greater than the number of entries" );
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
    hashKeys = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
    hashCodes = {0, 1, 4, 2, 3, 6, 0, 0, 1, 5};
//    for(unsigned i = 0;i<moveNum;++i){
//        ull n = distr(eng) & hashCodeMask;
//        while(any_of(hashCodes.begin(), hashCodes.begin() + i, [=](ull item){ return item == n; }))
//        { n = distr(eng) & hashCodeMask; }
//        hashCodes.push_back(n);

//        n = distr(eng);
//        while(any_of(hashKeys.begin(), hashKeys.begin() + i, [=](ull item){ return item == n; }))
//        { n = distr(eng); }
//        hashKeys.push_back(n);
//    }
}

template<typename T>
inline void ZHashTableBase<T>::update(unsigned moveIdx)
{
    // Zobrist hashing
    currCode ^= hashCodes[moveIdx];
    currKey ^= hashKeys[moveIdx];
}

template<typename T>
void ZHashTableBase<T>::backward(unsigned moveIdx)
{
    // XORing with the same value gives back the previous hash code and value
    update(moveIdx);
}

// ZHashTableBase<T> class is dependent so scope is not examined in derived classes during unqualified name lookup
// helper macro to use in derived classes
#define ZHASHTABLEBASE_TYPEDEFS typedef ZHashTableBase<T> Base; \
                                typedef typename ZHashTableBase<T>::HashNode HashNode; \
                                typedef typename ZHashTableBase<T>::ull ull; \

#endif // ZHASHTABLEBASE_H
