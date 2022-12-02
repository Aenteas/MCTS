#ifndef NODE_H
#define NODE_H

class Node
{
    ~Node()=delete;
    Node()=delete;
protected:

    Node(const Node&)=delete;
    Node& operator=(const Node&)=delete;
    Node(Node&&)=delete;
    Node& operator=(Node&&)=delete;

public:
    template<template<typename> typename T, typename N, typename G>
    static N* selectMostVisited(T<N>* const table, G* game);
};

template<template<typename> typename T, typename N, typename G>
N* Node::selectMostVisited(T<N>* const  table, G* game){
    double maxVisit = -1;
    unsigned int bestMoveIdx;
    N* bestChild;
    const auto& moves = game->getValidMoves().cbegin();
    while(moves){
        unsigned moveIdx = game->toMoveIdx(moves.getPiece(), moves.getPos());
        N* child = table->select(moveIdx);
        double visit = child ? child->getVisitCount() : 0;
        if(visit > maxVisit){
            maxVisit = visit;
            bestChild = child;
            bestMoveIdx = moveIdx;
        }
        ++moves;
    }
    if(bestChild)
        table->update(bestMoveIdx);
    game->update(bestMoveIdx);
    return bestChild;
}

#endif // NODE_H
