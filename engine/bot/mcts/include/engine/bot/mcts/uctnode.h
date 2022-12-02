#ifndef UCTNODE_H
#define UCTNODE_H

#include <iostream>
#include <vector>

/**********************************************************************************
 * UCTNode implementation (second variation) from                                 *
 * Childs, B. E., Brodeur, J. H., & Kocsis, L. (2008, December). Transpositions   *
 * and move groups in Monte Carlo tree search. In 2008 IEEE Symposium On          *
 * Computational Intelligence and Games (pp. 389-395). IEEE.                      *
 *                                                                                *
 * Hash table type could not be made as class template                            *
 * parameter because they are templated by the Node class leading infinite        *
 * template recursion.                                                            *
 *                                                                                *
 * Instead, we use the type to create template functions and pass                 *
 * it as argument. The cost of passing hash table as an argument each time is     *
 * negligible and probably will be optimized out by the compiler as they are      *
 * the same pointers during the whole search.                                     *
 * We make this intention clear to the compiler by marking them as constant       *
 * pointers.                                                                      *
 *                                                                                *
 * However, policy and game types should be made as class templates because these *
 * objects are used in the constructor and reset function, which is called from   *
 * the hashtable classes. Hashtable should not know about game and policy types   *
 **********************************************************************************/

template<typename G, typename P>
class UCTNode
{
public:
    UCTNode(UCTNode<G, P>* parent);

    UCTNode& operator=(const UCTNode&)=default;
    UCTNode(const UCTNode&)=default;
    UCTNode& operator=(UCTNode&&)=default;
    UCTNode(UCTNode&&)=default;
    ~UCTNode()=default;

    static void setup(G* game, Omega2* game2, P* policy);
    void reset(UCTNode<G, P>* parent);

    template<template<typename> typename T>
    UCTNode<G, P>* select(T<UCTNode<G, P>>* const table);

    template<template<typename> typename T>
    UCTNode<G, P>* expand(T<UCTNode<G, P>>* const table);

    template<template<typename> typename T>
    void backprop(double outcome, T<UCTNode<G, P>>* const table);

    void setupBackProp(unsigned depth);

    // getters
    double getStateScore() const;
    double getVisitCount() const;

    // c value for balancing exploration and exploitation
    static constexpr double c = 2.0;

    UCTNode<G, P>* parent;

protected:
    UCTNode(UCTNode& parent = nullptr);

    inline double actionScore(UCTNode<G,P>* child, unsigned int childIdx, double logc) const;

    double mean;
    double vCount;

    inline static P* policy;
    inline static G* game;
    inline static Omega2* game2;

    // We only store statistics for the available moves (children) to spare memory. As a result, we can not use
    // update items by direct move indexing (somewhat slower)
    std::vector<double> vCounts;
};

template<typename G, typename P>
void UCTNode<G, P>::setup(G* game, Omega2* game2, P* policy)
{
    UCTNode<G, P>::game = game;
    UCTNode<G, P>::game2 = game2;
    UCTNode<G, P>::policy = policy;
}

template<typename G, typename P>
UCTNode<G, P>::UCTNode(UCTNode<G, P>* parent):
    parent(parent),
    mean(0.5)
{
    vCount = game->getValidMoves().size();
    if(vCount != game2->getValidMoves().size())
        std::cout << "DIFF size uct" << std::endl;
    vCounts = std::vector<double> (vCount, 1);
}

template<typename G, typename P>
void UCTNode<G, P>::reset(UCTNode<G, P>* parent){
    this->parent = parent;
    mean = 0.5;
    vCount = game->getValidMoves().size();
    if(vCount != game2->getValidMoves().size())
        std::cout << "DIFF validmovesize uct" << std::endl;
    // here there is a potential for heap allocation when the number of valid moves can increase in a new position like in chess
    // in other games like gomoku and omega this is not the case
    // alternatively we could preallocate size to the maximum number of valid moves to avoid heap allocation but
    // that would be at the expense of increasing memory footprint
    vCounts.resize(vCount);
    fill(vCounts.begin(), vCounts.end(), 1);
}

template<typename G, typename P>
double UCTNode<G, P>::actionScore(UCTNode<G, P>* child, unsigned int childIdx, double logc) const {
    return (child ? child->mean : 0.5) + sqrt(logc / vCounts[childIdx]);
}

template<typename G, typename P>
template<template<typename> typename T>
UCTNode<G, P>* UCTNode<G, P>::select(T<UCTNode<G, P>>* const table){
    UCTNode<G, P>* bestChild = nullptr;
    unsigned int bestIdx;
    unsigned bestMoveIdx;
    double maxScore = -1;
    double score;
    unsigned int idx=0;
    double logc = c * log(vCount + 1);
    const auto& moves = game->getValidMoves().cbegin();
    auto moves2 = game2->getValidMoves();
    auto moves2it = moves2.begin();
    while(moves){
        auto& move2 = *moves2it;
        unsigned moveIdx = game->toMoveIdx(moves.getPiece(), moves.getPos());
        if(move2.piece != moves.getPiece())
            std::cout << "DIFF piece uct" << std::endl;
        if(move2.idx != moves.getPos())
            std::cout << "DIFF pos uct" << std::endl; 
        if(game2->getMoveIdx(move2.piece, move2.idx) != moveIdx)
            std::cout << "DIFF conv uct" << std::endl;
        
        UCTNode<G, P>* child = table->select(moveIdx);
        score = actionScore(child, idx, logc);
        if(score > maxScore){
            maxScore = score;
            bestChild = child;
            bestMoveIdx = moveIdx;
            bestIdx = idx;
        }
        ++idx;
        ++moves;
        ++moves2it;
    }

    // When we choose to visit an unexplored state we stop the selection phase and will expand the node with the new child
    // During expansion we will update the table by calling store on it so no need to update it here in that case
    if(bestChild)
        table->update(bestMoveIdx);
    game->select(bestMoveIdx);
    game2->update(bestMoveIdx);
    // update visit counts
    ++vCount;
    ++vCounts[bestIdx];
    return bestChild;
}

template<typename G, typename P>
template<template<typename> typename T>
UCTNode<G, P>* UCTNode<G, P>::expand(T<UCTNode<G, P>>* const table) {
    unsigned moveIdx = game->getLastMoveIdx();
    if(moveIdx != game2->getLastMoveIdx())
        std::cout << "DIFF last uct" << std::endl;
    UCTNode<G, P>* leaf = table->store(moveIdx);
    // simulate an action from leaf
    if(game->end() != game2->end())
        std::cout << "DIFF end uct" << std::endl;
    if(!game->end()){
        auto [_, childIdx] = policy->select();
        // update child statistics for leaf
        ++leaf->vCount;
        ++leaf->vCounts[childIdx];
    }
    return leaf;
}

template<typename G, typename P>
template<template<typename> typename T>
void UCTNode<G, P>::backprop(double outcome, T<UCTNode<G, P>>* const table){
    UCTNode<G, P>* current = this;
    UCTNode<G, P>* currParent = current->parent;
    std::cout << "backprop: " << std::endl;
    while(currParent){
        game->undo();
        game2->undo();
        // win: 1, draw: 0.5, lose: 0
        // Outcome is from the WHITE player's perspective, val is from the current player's perspective
        double val = outcome+game->getNextPlayer()*(1.0-2.0*outcome);
        std::cout << "backprop val: " << val << " depth:" << game2->getCurrentDepth() << std::endl;
        if(game->getNextPlayer() != game2->getCurrentPlayer())
            std::cout << "DIFF cplayer uct" << std::endl;
        current->mean = (current->mean*(current->vCount-1)+val)/(current->vCount);
        current = currParent;
        currParent = current->parent;
    }
    table->selectRoot();
}

template<typename G, typename P>
double UCTNode<G, P>::getStateScore() const {
    return mean;
}

template<typename G, typename P>
double UCTNode<G, P>::getVisitCount() const {
    return vCount;
}

template<typename G, typename P>
void UCTNode<G, P>::setupBackProp(unsigned depth){
    // update game with the leaf node state as preparation for backprop
    while(game->getCurrentDepth() != depth){
        game->undo();
        game2->undo();
        if(game2->getCurrentDepth() != game->getCurrentDepth())
            std::cout << "DIFF depth uct" << std::endl;
    }
}

#endif // UCTNODE_H
