#ifndef NODE_H
#define NODE_H

#include "engine/game/omega/omega2.h"

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
    static N* selectMostVisited(T<N>* const table, G* game, Omega2* game2);
};

template<template<typename> typename T, typename N, typename G>
N* Node::selectMostVisited(T<N>* const  table, G* game, Omega2* game2){
    double maxVisit = -1;
    unsigned int bestMoveIdx;
    N* bestChild;
    const auto& moves = game2->getValidMoves().cbegin();
    for(unsigned int moveIdx : game->getValidMoveIdxs()){
        if(game2->toMoveIdx(moves.getPiece(), moves.getPos()) != moveIdx)
            std::cout << "DIFF node" << std::endl;
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
    game2->update(bestMoveIdx);
    return bestChild;
}

#endif // NODE_H
