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
class UCTNode: public Node<T, G>
{
    typedef Node<T,G> Node;
public:
    UCTNode(UCTNode<G, P>* parent);

    UCTNode& operator=(const UCTNode&)=default;
    UCTNode(const UCTNode&)=default;
    UCTNode& operator=(const UCTNode&&)=default;
    UCTNode(const UCTNode&&)=default;

    static void setup(G* game, P* policy);
    void reset(UCTNode<G, P>* parent);

    template<template<typename> typename T>
    UCTNode<G, P>* select(T<UCTNode<G, P>>* const table);

    template<template<typename> typename T>
    UCTNode<G, P>* expand(T<UCTNode<G, P>>* const table);

    template<template<typename> typename T>
    UCTNode<G, P>* backprop(double outcome, T<UCTNode<G, P>>* const table);

    static UCTNode<G, P>* backward();

    // getters
    double getStateScore() const;
    double getVisitCount() const;

    // c value for balancing exploration and exploitation
    static constexpr double c = 2.0;
    static constexpr double initialvCount = 1.0;

protected:
    UCTNode(UCTNode& parent = nullptr);
    ~UCTNode()=default;

    inline double actionScore(UCTNode<G, P>* child, unsigned int moveIdx, unsigned int childIdx, double logc) const;

    double mean;
    double vCount;
    // We only store statistics for the available moves (children)
    vector<double> vCounts;

    UCTNode<G, P>* parent;
    thread_local static P* policy;
    thread_local static G* game;
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
    mean = G::depth > 0 ? UCTNode<G, P>::policy->getScore(UCTNode<G, P>::game->getLastMove(), UCTNode<G, P>::game->getPreviousPlayer()) : 0.5;
    unsigned numChild = UCTNode<G, P>::game->getValidMoves().size();
    vCount = UCTNode<G, P>::initialvCount * numChild;
    vCounts = vector<double> (numChild, UCTNode<G, P>::initialvCount);
}

template<typename G, typename P>
void UCTNode<G, P>::reset(UCTNode<G, P>* parent){
    this->parent = parent;
    // no need to check depth because reset will only be called when depth > 0
    mean = UCTNode<G, P>::policy->getScore(UCTNode<G, P>::game->getLastMove(), UCTNode<G, P>::game->getPreviousPlayer());
    fill(vCounts.begin(), vCounts.end(), UCTNode<G, P>::initialvCount);
}

template<typename G, typename P>
double UCTNode<G, P>::actionScore(UCTNode<G, P>* child, unsigned int moveIdx, unsigned int childIdx, double logc) const {
    return (child ? child->mean : Node<T>::policy->getScore(moveIdx, playerColor)) + sqrt(UCTNode<G, P>::logc / vCounts[childIdx]);
}

template<typename G, typename P>
template<template<typename> typename T>
UCTNode<G, P>* UCTNode<G, P>::select(T<UCTNode<G, P>>* const table){
    double maxScore = -1;
    double score;
    unsigned int bestMoveIdx;
    UCTNode<G, P>* bestChild;
    unsigned int idx=0;
    unsigned int bestIdx;
    UCTNode<G, P>::logc = UCTNode<G, P>::c * log(vCount + 1);
    for(unsigned int moveIdx : UCTNode<G, P>::game->getValidMoveIdxs()){
        UCTNode<G, P>* child = UCTNode<G, P>::tTable->load(moveIdx);
        score = actionScore(child, moveIdx, idx, logc);
        if(score > maxScore){
            maxScore = score;
            bestChild = child;
            bestMoveIdx = moveIdx;
            bestIdx = idx;
        }
        // xor twice with the same value gives back the original
        UCTNode<G, P>::tTable->backward(moveIdx);
        ++idx;
    }
    // update visit counts
    ++vCount;
    ++vCounts[bestIdx];
    // visit the node, bestMoveIdx will always be updated at this point
    UCTNode<G, P>::game->update(bestMoveIdx);
    UCTNode<G, P>::table->update(bestMoveIdx);
    return bestChild;
}

template<typename G, typename P>
template<template<typename> typename T>
UCTNode<G, P>* UCTNode<G, P>::expand(T<UCTNode<G, P>>* const table) {
    UCTNode<G, P>::table->store();
    auto [moveIdx, childIdx] = UCTNode<G, P>::policy->select();
    // update child statistics for leaf
    ++vCount;
    ++vCounts[childIdx];
    gameState->update(moveIdx);
    // return simulated child
    return UCTNode<G, P>::table->select(moveIdx);
}

template<typename G, typename P>
template<template<typename> typename T>
void UCTNode<G, P>::backprop(double outcome, T<UCTNode<G, P>>* const table){
    if(parent){
        UCTNode<G, P>::game->undo();
        double val = outcome+UCTNode<G, P>::game->getCurrentPlayer()*(1.0-2.0*outcome);
        mean = (mean*(vCount-1)+val)/(vCount);
        parent->backprop(outcome, table);
    }
    else
        UCTNode<G, P>::table->selectRoot();
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
void UCTNode<G, P>::backward(){
    UCTNode<G, P>::game->undo();
}

#endif // UCTNODE_H
