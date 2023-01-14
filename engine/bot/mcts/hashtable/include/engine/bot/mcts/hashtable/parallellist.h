#ifndef PARALLELLIST_H
#define PARALLELLIST_H

#include <mutex>
#include <memory>

template<typename T>
class ParallelList{
public:
    class Node
    {
        friend class ParallelList<T>;
    private:
        Node* next;
        Node* prev;

        template<class... Args>
        Node(Args&&... args):
            data(T(std::forward<Args>(args)...)) {}

        ~Node()=default;
        
    public:
        T data;
    };

    template<class... Args>
    ParallelList(unsigned size, Args&&... args)
    {
        if(size >= 2){
            auto* p = new Node(std::forward<Args>(args)...);
            dummyFront = p;
            dummyFront->prev = nullptr;
            for(unsigned i = 0; i < size + 1; ++i){
                p->next = new Node(std::forward<Args>(args)...);
                p->next->prev = p;
                p = p->next;
            }
            _front = dummyFront->next;
            dummyBack = p;
            dummyBack->next = nullptr;
            _back = dummyBack->prev;
            // T is a hashNode in our case, last is root
            _back->data.code = 0;
        }
        else
            throw std::invalid_argument( "ParallelList: size should be at least 2." );
    }

    ~ParallelList(){
        auto* p = dummyFront;
        while(p){
            auto* temp = p->next;
            delete p;
            p = temp;
        }
    }

    ParallelList(const ParallelList& other)=delete;
    ParallelList& operator=(const ParallelList& other)=delete;

    Node* splice(Node* source, Node* target){
        // child node can not have the same haskey as parent so we do not need to check this
        // only XOR-ing with pure 0s would do that but we use a trick to eliminate pure 0s from Base::hashKeys
        // if(source == target)
        //     return;
        std::lock_guard<std::mutex> lock(m);
        Node* p = source;
        // remove
        source->prev->next = source->next; // source should always have prev as we have a dummy node at the front
        source->next->prev = source->prev; // source should always have next because the way we use this class
        // add before target
        target->prev->next = source; // target should always have prev because the way the list is used
        source->prev = target->prev;
        target->prev = source;
        source->next = target;
        // update front and back
        _front = dummyFront->next;
        _back = dummyBack->prev;
        // only data is accessable from Node so it is safe to return
        return p;
    }

    Node* spliceFront(Node* target){
        std::lock_guard<std::mutex> lock(m);
        Node* p = _front;
        // remove
        dummyFront->next = _front->next;
        _front->next->prev = dummyFront;
        // add before target
        target->prev->next = _front;
        _front->prev = target->prev;
        target->prev = _front;
        _front->next = target;
        // update front and back
        _front = dummyFront->next;
        _back = dummyBack->prev;
        // only data is accessable from Node so it is safe to return
        return p;
    }
    void spliceRoot(){
        // not used concurrently so no lock is needed
        // move last to front
        dummyBack->prev = _back->prev;
        _back->prev->next = dummyBack;
        dummyFront->next = _back;
        _front->prev = _back;
        _back->prev = dummyFront;
        _back->next = _front;
        _front = _back;
        _back = dummyBack->prev;
    }
    Node* end(){
        // only data is accessable from Node so it is safe to return
        return dummyBack;
    }
    Node* back(){
        // need a lock to safely make a copy
        std::lock_guard<std::mutex> lock(m);
        // only data is accessable from Node so it is safe to return
        auto res = _back;
        return res;
    }
    // we do not write _front from concurrent threads
    Node* _front;
private:
    Node* _back;
    Node* dummyFront;
    Node* dummyBack;

    std::mutex m;
};

#endif // PARALLELLIST_H