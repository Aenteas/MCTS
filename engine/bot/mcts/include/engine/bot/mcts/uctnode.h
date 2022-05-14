#ifndef UCTNODE_H
#define UCTNODE_H

#include <vector>

using namespace std;

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

    static void setup(G* game, P* policy);
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

    inline double actionScore(UCTNode<G, P>* child, typename G::Move move, unsigned int childIdx, double logc) const;

    double mean;
    double vCount;

    inline thread_local static P* policy;
    inline thread_local static G* game;

    // We only store statistics for the available moves (children)
    vector<double> vCounts;
};

template<typename G, typename P>
void UCTNode<G, P>::setup(G* game, P* policy)
{
    UCTNode<G, P>::game = game;
    UCTNode<G, P>::policy = policy;
}

template<typename G, typename P>
UCTNode<G, P>::UCTNode(UCTNode<G, P>* parent):
    parent(parent)
{
    mean = game->getCurrentDepth() > 0 ? policy->getScore(game->getLastMove()) : 0.5;
    vCount = game->getValidMoves().size();
    vCounts = vector<double> (vCount, 1);
}

template<typename G, typename P>
void UCTNode<G, P>::reset(UCTNode<G, P>* parent){
    this->parent = parent;
    // no need to check depth because reset will only be called when depth > 0
    mean = policy->getScore(game->getLastMove());
    vCount = game->getValidMoves().size();
    vCounts.resize(vCount);
    fill(vCounts.begin(), vCounts.end(), 1);
}

template<typename G, typename P>
double UCTNode<G, P>::actionScore(UCTNode<G, P>* child, typename G::Move move, unsigned int childIdx, double logc) const {
    return (child ? child->mean : policy->getScore(move)) + sqrt(logc / vCounts[childIdx]);
}

template<typename G, typename P>
template<template<typename> typename T>
UCTNode<G, P>* UCTNode<G, P>::select(T<UCTNode<G, P>>* const table){
    UCTNode<G, P>* bestChild = nullptr;
    unsigned int bestIdx;
    // first move will be chosen if there is no child
    unsigned bestMoveIdx;
    // usually the number of available moves are higher than the search depth so it improves the performance if we do a check here
    if(vCount > vCounts.size()){
        double maxScore = -1;
        double score;
        unsigned int idx=0;
        double logc = c * log(vCount + 1);
        for(const auto& move : game->getValidMoves()){
            unsigned moveIdx = game->getMoveIdx(move.piece, move.idx);
            UCTNode<G, P>* child = table->select(moveIdx);
            score = actionScore(child, move, idx, logc);
            if(score > maxScore){
                maxScore = score;
                bestChild = child;
                bestMoveIdx = moveIdx;
                bestIdx = idx;
            }
            ++idx;
        }
        // only update table if child is found. This is because
        // we need to expand before updating parent from the table
        // we need to check again if there is any child of the parent as
        // children could be removed from transposition table

        if(bestChild)
            table->update(bestMoveIdx);
        game->update(bestMoveIdx);
    }
    else{
        auto [moveIdx, idx] = policy->select();
        bestMoveIdx = moveIdx;
        bestIdx = idx;
    }
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
void UCTNode<G, P>::backprop(double outcome, T<UCTNode<G, P>>* const table){
    UCTNode<G, P>* current = this;
    UCTNode<G, P>* currParent = current->parent;
    while(currParent){
        game->undo();
        double val = outcome+game->getCurrentPlayer()*(1.0-2.0*outcome);
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
    while(game->getCurrentDepth() != depth)
        game->undo();
}

#endif // UCTNODE_H