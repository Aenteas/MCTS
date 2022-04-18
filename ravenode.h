#ifndef RAVENODE_H
#define RAVENODE_H

#include <vector>
#include <array>

using namespace std;

/**********************************************************************************
 * RAVENode                                                                       *
 * Implements rapid value estimation:                                             *
 * https://www.cs.utexas.edu/~pstone/Courses/394Rspring11/resources/mcrave.pdf    *
 *                                                                                *
 * Moves are indexed by piece id and move index instead of a single move index.   *
 * This is much more memory friendly as we only store the indices for available   *
 * pieces. The cost is that we need double indexing but it is negligible          *
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
class RAVENode
{
public:
    RAVENode(RAVENode<G, P>* parent);

    RAVENode<G, P>& operator=(const RAVENode<G, P>&)=default;
    RAVENode(const RAVENode<G, P>&)=default;
    RAVENode<G, P>& operator=(const RAVENode<G, P>&&)=default;
    RAVENode(const RAVENode<G, P>&&)=default;

    static void setup(G* game, P* policy);

    void reset(RAVENode<G, P>* parent);

    template<template<typename> typename T>
    RAVENode<G, P>* select(T<RAVENode<G, P>>* const table);

    template<template<typename> typename T>
    RAVENode<G, P>* expand(T<RAVENode<G, P>* const table);

    template<template<typename> typename T>
    RAVENode<G, P>* backprop(double outcome, T<RAVENode<G, P>* const table);

    static RAVENode<G, P>* backward();

    // getters
    double getStateScore() const;
    double getVisitCount() const;

protected:
    RAVENode(RAVENode<G, P>* parent = nullptr);
    ~RAVENode()=default;

    inline void updateMC(double val);
    inline void updateRAVE(double outcome);

    double actionScore(RAVENode<G, P>* child, unsigned moveIdx) const;

    // k value for weigthing MC and AMAF values
    static constexpr double k = 500;

    // MC values are stored at the child nodes so they get more samples
    double mcMean;
    double mcCount;

    // AMAF values are stored at the parent to spare memory
    /**
     * Why do we store all the possible moves for a piece? Because for RAVE,
     * during backpropagation we update each node with the moves that were
     * played later on. If we stored only the valid moves then the same moves
     * might be indexed differently for different nodes
    **/
    // [pieceid][moveidx]
    array<vector<double>, G::PIECENUM> rMean;
    array<vector<double>, G::PIECENUM> rCount;
    // moves to update during backpropagation: [playercolor][pieceid][moveidx]
    thread_local inline static array<array<vector<short>, G::PIECENUM>, 2> takenMoves;

    RAVENode<G, P>* parent;
    thread_local static P* policy;
    thread_local static G* game;
};

template<typename G, typename P>
void RAVENode<G, P>::setup(G* game, P* policy)
{
    RAVENode<G, P>::game = game;
    RAVENode<G, P>::policy = policy;
    for(const auto& playerMoves : RAVENode<G, P>::takenMoves){
        for(const auto& pieceMoves : playerMoves){
            pieceMoves.reserve(G::MAXDEPTH); // reserve so we do not need to reallocate later
        }
    }
}
// takenMove -> lastMove
template<typename G, typename P>
RAVENode<G, P>::RAVENode(RAVENode<G, P>* parent):
    parent(parent),
    mcCount(1)
{
    mcMean = RAVENode<G, P>::game->getCurrentDepth() > 0 ? RAVENode<G, P>::policy->getScore(RAVENode<G, P>::game->getLastMove(), RAVENode<G, P>::game->getPreviousPlayer()) : 0.5;
    // assign initial values from the default policy
    rMean = RAVENode<G, P>::policy->getScores(RAVENode<G, P>::game->getCurrentPlayer());
    // confidence is given by the number of equivalent samples
    for(unsigned piece : RAVENode<G, P>::game->getAvailablePieces()){
        rCount[piece] = vector<double>(rMean[piece].size(), 1);
    }
}
// lastMoveIdx
template<typename G, typename P>
void RAVENode<G, P>::reset(RAVENode<G, P>* parent){
    this->parent = parent;
    mcCount = 1;
    // no need to check depth because reset will only be called when depth > 0
    mcMean = RAVENode<G, P>::policy->getScore(RAVENode<G, P>::game->getLastMoveIdx(), RAVENode<G, P>::game->getPreviousPlayer());
    rMean = RAVENode<G, P>::policy->getScores(RAVENode<G, P>::game->getCurrentPlayer());
    for(unsigned piece : RAVENode<G, P>::game->getAvailablePieces()){
        fill(rCount[piece].begin(), rCount[piece].end(), 1);
    }
}

template<typename G, typename P>
double RAVENode<G, P>::actionScore(RAVENode<G, P>* child, unsigned moveIdx) const {
    double beta = sqrt(RAVENode<G, P>::k / ((child ? child->mcCount : 0) + RAVENode<G, P>::k));
    double score = (1-beta) *
    (child ? child->mcMean : RAVENode<G, P>::policy->getScore(moveIdx, RAVENode<G, P>::game->getCurrentPlayer()))
    + beta * (rMean[piece][RAVENode<G, P>::game->getPieceMoveIdx(moveIdx)]);
    return score;
}
// RAVENode<G, P>::game->getValidMoveIdxs()
template<typename G, typename P>
template<template<typename> typename T>
RAVENode<G, P>* RAVENode<G, P>::select(T<RAVENode<G, P>>* const table){
    double maxScore = -1;
    double score;
    unsigned bestMoveIdx;
    RAVENode<G, P>* bestChild = nullptr;
    for(unsigned moveIdx : RAVENode<G, P>::game->getValidMoveIdxs()){
        RAVENode<G, P>* child = table->load(moveIdx);
        score = actionScore(child, moveIdx);
        if(score > maxScore){
            maxScore = score;
            bestChild = child;
            bestMoveIdx = moveIdx;
        }
        // xor twice with the same value gives back the original
        table->backward(moveIdx);
    }
    // visit the node, bestMoveIdx will always be updated at this point
    RAVENode<G, P>::game->update(bestMoveIdx);
    table->update(bestMoveIdx);
    return bestChild;
}

template<typename G, typename P>
template<template<typename> typename T>
RAVENode<G, P>* RAVENode<G, P>::expand(T<RAVENode<G, P>>* const table) {
    return table->store();
}

template<typename G, typename P>
void RAVENode<G, P>::updateMC(double val){
    // MC values are stored at child nodes to have more samples
    mcMean = (mcMean*mcCount+val)/(mcCount+1);
    ++mcCount;
}
// getAvailablePieces
template<typename G, typename P>
void RAVENode<G, P>::updateRAVE(double outcome){
    auto player = RAVENode<G, P>::game->getCurrentPlayer();
    val = outcome+player*(1.0-2.0*outcome);
    // update moves taken by player with available pieces
    for(unsigned piece : RAVENode<G, P>::game->getAvailablePieces()){
        for(unsigned moveIdx : RAVENode<G, P>::takenMoves[player][piece]){
            rMean[piece][moveIdx] = (rMean[piece][moveIdx] * rCount[piece][moveIdx]+val)/(rCount[piece][moveIdx]+1);
            ++rCount[piece][moveIdx];
        }
    }
}
// getLastMove, getLastMoveIdx, x getCurrentColor
template<typename G, typename P>
template<template<typename> typename T>
void RAVENode<G, P>::backprop(double outcome, T<RAVENode<G, P>>* const table){
    // action value is updated with the current player
    updateRAVE(outcome);
    // backpropagate parent
    if(parent){
        // state value is updated with previous player
        auto move = RAVENode<G, P>::game->getLastMove();
        updateMC(outcome+move.player*(1.0-2.0*outcome));
        RAVENode<G, P>::takenMoves[move.player][move.piece].push_back(move.idx);
        parent->backprop(outcome, table);
        // visit previous game state
        RAVENode<G, P>::game->undo();
    }
    else{ // empty takenMoves at root
        for(const auto& playerMoves : RAVENode<G, P>::takenMoves){
            for(const auto& pieceMoves : playerMoves){
                pieceMoves.clear();
            }
        }
        // set table to root
        table->selectRoot();
    }
}

template<typename G, typename P>
double RAVENode<G, P>::getStateScore() const {
    return mcMean;
}

template<typename G, typename P>
double RAVENode<G, P>::getVisitCount() const {
    return mcCount;
}

template<typename G, typename P>
void RAVENode<G, P>::backward(){
    auto move = RAVENode<G, P>::game->getLastMove();
    RAVENode<G, P>::takenMoves[move.player][move.piece].push_back(move.idx);
    RAVENode<G, P>::game->undo();
}

#endif // RAVENODE_H
