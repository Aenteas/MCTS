#ifndef UCTNODE_H
#define UCTNODE_H

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
    UCTNode(UCTNode<G, P>* parent = nullptr);

    UCTNode& operator=(const UCTNode&)=default;
    UCTNode(const UCTNode&)=default;
    UCTNode& operator=(UCTNode&&)=default;
    UCTNode(UCTNode&&)=default;
    ~UCTNode()=default;

    static void setup(G* game, P* policy);
    void reset(UCTNode<G, P>* parent);

    template<template<typename> typename T>
    UCTNode<G, P>* select(T<UCTNode<G, P>>* const table);

    template<template<typename> typename T>
    UCTNode<G, P>* expand(T<UCTNode<G, P>>* const table);

    template<template<typename> typename T>
    void backprop(double outcome, T<UCTNode<G, P>>* const table, unsigned leafDepth);

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

    // We only store statistics for the available moves (children) to spare memory. As a result, we can not use
    // update items by direct move indexing (somewhat slower)
    std::vector<double> vCounts;
};

template<typename G, typename P>
void UCTNode<G, P>::setup(G* game, P* policy)
{
    UCTNode<G, P>::game = game;
    UCTNode<G, P>::policy = policy;
}

template<typename G, typename P>
void UCTNode<G, P>::reset(UCTNode<G, P>* parent){
    this->parent = parent;
    mean = 0.5;
    vCount = game->getValidMoves().size();
    // here there is a potential for heap allocation when the number of valid moves can increase in a new position like in chess
    // in other games like gomoku and omega this is not the case
    // alternatively we could preallocate size to the maximum number of valid moves to avoid heap allocation but
    // that would be at the expense of increasing memory footprint
    vCounts.resize(vCount);
    fill(vCounts.begin(), vCounts.end(), 1);
}

template<typename G, typename P>
UCTNode<G, P>::UCTNode(UCTNode<G, P>* parent)
{
    reset(parent);
}

template<typename G, typename P>
double UCTNode<G, P>::actionScore(UCTNode<G, P>* child, unsigned int childIdx, double logc) const {
    return (child ? child->mean : 0.5) + sqrt(logc / vCounts[childIdx]);
}

template<typename G, typename P>
template<template<typename> typename T>
UCTNode<G, P>* UCTNode<G, P>::select(T<UCTNode<G, P>>* const table){
    UCTNode<G, P>* bestChild = nullptr;
    unsigned bestIdx;
    unsigned bestMoveIdx;
    double maxScore = -1;
    double score;
    unsigned idx=0;
    double logc = c * log(vCount + 1);
    for(const auto& move : game->getValidMoves()){
        unsigned moveIdx = game->toMoveIdx(move.getPiece(), move.getPos());
        UCTNode<G, P>* child = table->select(moveIdx);
        score = actionScore(child, idx, logc);
        if(score > maxScore){
            maxScore = score;
            bestChild = child;
            bestMoveIdx = moveIdx;
            bestIdx = idx;
        }
        ++idx;
    }

    // When we choose to visit an unexplored state we stop the selection phase and will expand the node with the new child
    // During expansion we will update the table by calling store on it so no need to update it here in that case
    if(bestChild)
        table->update(bestMoveIdx);
    game->select(bestMoveIdx);
    // update visit counts
    ++vCount;
    ++vCounts[bestIdx];
    return bestChild;
}

template<typename G, typename P>
template<template<typename> typename T>
UCTNode<G, P>* UCTNode<G, P>::expand(T<UCTNode<G, P>>* const table) {
    unsigned moveIdx = game->getLastMoveIdx();
    UCTNode<G, P>* leaf = table->store(moveIdx);
    // simulate an action from leaf
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
void UCTNode<G, P>::backprop(double outcome, T<UCTNode<G, P>>* const table, unsigned leafDepth){
    // go up to leaf
    while(game->getCurrentDepth() != leafDepth){
        game->undo();
    }
    // backprop
    UCTNode<G, P>* current = this;
    UCTNode<G, P>* currParent = current->parent;
    while(currParent){
        game->undo();
        // win: 1, draw: 0.5, lose: 0
        // Outcome is from the WHITE player's perspective, val is from the current player's perspective
        double val = outcome+game->getNextPlayer()*(1.0-2.0*outcome);
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

#endif // UCTNODE_H
