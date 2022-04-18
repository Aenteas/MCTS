#ifndef NODE_H
#define NODE_H

class Node
{
protected:
    ~Node()=default;

    Node(const Node&)=delete;
    Node& operator=(const Node&)=delete;
    Node(const Node&&)=delete;
    Node& operator=(const Node&&)=delete;

public:
    template<template<typename> typename T, typename N, typename G>
    static N* selectMostVisited(T<N>* table, G* game);
};

template<template<typename> typename T, typename N, typename G>
N* Node::selectMostVisited(T<N>* table, G* game){
    double maxVisit = -1;
    unsigned int bestMoveIdx;
    N* bestChild;
    for(unsigned int moveIdx : game->getValidMoves()){
        N* child = table->load(moveIdx);
        double visit = child ? child->getVisitCount() : 0;
        if(visit > maxVisit){
            maxVisit = visit;
            bestChild = child;
            bestMoveIdx = moveIdx;
        }
        table->backward(moveIdx);
    }
    game->update(bestMoveIdx);
    table->update(bestMoveIdx);
    return bestChild;
}

#endif // NODE_H
