#ifndef PARALLELLIST_H
#define PARALLELLIST_H

#include <mutex>
#include <memory>

template<typename T>
class ParallelList{
    class Node
    {
    private:
        std::mutex mNext;
        std::mutex mPrev;
        unsigned idx;
        
        Node* next;
        Node* prev;

        template<class... Args>
        Node(Args&&... args):
            data(T(std::forward<Args>(args)...)) {}

        ~Node()=default;
        
    public:
        T data;
    };

    Node* dummyFront;
    Node* dummyBack;
    Node* front;
    Node* back;
public:

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
            front = dummyFront->next;
            dummyBack = p;
            dummyBack->next = nullptr;
            back = dummyBack->back;
            // T is a hashNode in our case, last is root
            back->data.code = 0;
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

    void splice(Node* source, Node* target){
        // child node can not have the same haskey as parent so we do not need to check this
        // only XOR-ing with pure 0s would do that but we use a trick to eliminate pure 0s from Base::hashKeys
        // if(source == target)
        //     return;
        // mutual exclusivity for modifying source and its previous node -> safe to read/write source->prev
        // lock simultaneously to avoid deadlocks
        std::lock(source->mNext, source->mPrev, target->mPrev, source->next->mPrev);
        // remove
        source->prev->next = source->next; // source should always have prev as we have a dummy node at the front
        source->next->prev = source->prev; // source should always have next because the way we use this class
        // add before target
        target->prev->next = source; // target should always have prev because the way the list is used
        source->prev = target->prev;
        target->prev = source;
        source->next = target;
        // update front and back
        front = dummyFront->next;
        back = dummyBack->back;
        // unlock
        source->mNext.unlock();
        source->mPrev.unlock();
        target->mPrev.unlock();
        source->next->mPrev.unlock();
    }

    Node* spliceFront(Node* target){
        std::lock(front->mNext, front->mPrev, target->mPrev, front->next->mPrev);
        Node* p = front;
        // remove
        dummyFront->next = front->next;
        front->next->prev = dummyFront;
        // add before target
        target->prev->next = front;
        front->prev = target->prev;
        target->prev = front;
        front->next = target;
        // update front and back
        front = dummyFront->next;
        back = dummyBack->back;
        // unlock
        front->mNext.unlock();
        front->mPrev.unlock();
        target->mPrev.unlock();
        front->next->mPrev.unlock();
        // only data is accessable from Node so it is safe to return
        return p;
    }
};

#endif // PARALLELLIST_H