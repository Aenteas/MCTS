#ifndef RAVENODE_H
#define RAVENODE_H

#include <vector>
#include <array>

/**********************************************************************************
 * RAVENode                                                                       *
 * Implements rapid value estimation:                                             *
 * https://www.cs.utexas.edu/~pstone/Courses/394Rspring11/resources/mcrave.pdf    *
 * It is mostly effective for games where the currently available moves are       *
 * available in the subsequent turns if they are not played in the current turn   *
 * like go, hex, omega (unlike chess)                                             *
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
    RAVENode();

    RAVENode& operator=(const RAVENode&)=default;
    RAVENode(const RAVENode&)=default;
    RAVENode& operator=(RAVENode&&)=default;
    RAVENode(RAVENode&&)=default;
    ~RAVENode()=default;

    static void setup(G* game, P* policy);

    void reset();

    template<template<typename> typename T>
    RAVENode<G, P>* select(T<RAVENode<G, P>>* const table);

    template<template<typename> typename T>
    RAVENode<G, P>* expand(T<RAVENode<G, P>>* const table);

    template<template<typename> typename T>
    void backprop(double outcome, T<RAVENode<G, P>>* const table, unsigned leafDepth);

    // getters
    double getStateScore() const;
    double getVisitCount() const;
protected:

    inline void updateMC(double val);
    inline void updateRAVE(double outcome, const std::array<std::vector<unsigned>, 2>& takenMoves);

    inline double actionScore(RAVENode<G, P>* child, double beta, unsigned idx) const;

    // k value for weigthing MC and AMAF values
    static constexpr double k = 1000;

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
    // [piece][pos]
    std::vector<double> rMean;
    std::vector<double> rCount;

    inline static P* policy;
    inline static G* game;
};

template<typename G, typename P>
void RAVENode<G, P>::setup(G* game, P* policy)
{
    RAVENode<G, P>::game = game;
    RAVENode<G, P>::policy = policy;
}

template<typename G, typename P>
void RAVENode<G, P>::reset(){
    mcCount = 1;
    mcMean = 0.5;
    std::fill(rCount.begin(), rCount.end(), 1);
    std::fill(rMean.begin(), rMean.end(), 0.5);
}

template<typename G, typename P>
RAVENode<G, P>::RAVENode():
    mcCount(1.0),
    mcMean(0.5),
    rCount(std::vector<double>(game->getTotalValidMoveNum(), 1)),
    rMean(std::vector<double>(game->getTotalValidMoveNum(), 0.5))
{
}

template<typename G, typename P>
double RAVENode<G, P>::actionScore(RAVENode<G, P>* child, double beta, unsigned idx) const {
    return (1-beta) *
    (child ? child->mcMean : 0.5)
    + beta * (rMean[idx]);
}

template<typename G, typename P>
template<template<typename> typename T>
RAVENode<G, P>* RAVENode<G, P>::select(T<RAVENode<G, P>>* const table){
    RAVENode<G, P>* bestChild = nullptr;
    unsigned bestIdx;
    unsigned bestMoveIdx;
    double maxScore = -1;
    double score;
    double beta = sqrt(RAVENode<G, P>::k / (3.0 * mcCount + RAVENode<G, P>::k));
    for(const auto& move : game->getValidMoves()){
        unsigned piece = move.getPiece();
        unsigned pos = move.getPos();
        unsigned moveIdx = game->toMoveIdx(piece, pos);
        RAVENode<G, P>* child = table->select(moveIdx);
        score = actionScore(child, beta, moveIdx);
        if(score > maxScore){
            maxScore = score;
            bestChild = child;
            bestMoveIdx = moveIdx;
        }
    }
    // When we choose to visit an unexplored state we stop the selection phase and will expand the node with the new child
    // During expansion we will update the table by calling store on it so no need to update it here in that case
    if(bestChild)
        table->update(bestMoveIdx);
    game->select(bestMoveIdx);
    return bestChild;
}

template<typename G, typename P>
template<template<typename> typename T>
RAVENode<G, P>* RAVENode<G, P>::expand(T<RAVENode<G, P>>* const table) {
    unsigned moveIdx = game->getLastMoveIdx();
    return table->store(moveIdx);
}

template<typename G, typename P>
void RAVENode<G, P>::updateMC(double val){
    mcMean = (mcMean*mcCount+val)/(mcCount+1);
    ++mcCount;
}

template<typename G, typename P>
void RAVENode<G, P>::updateRAVE(double outcome, const std::array<std::vector<unsigned>, 2>& takenMoves){
    auto player = game->getNextPlayer();
    double val = outcome+player*(1.0-2.0*outcome);
    // update available moves with the ones that were taken
    for(unsigned idx : takenMoves[player]){
        rMean[idx] = (rMean[idx] * rCount[idx]+val)/(rCount[idx]+1);
        ++rCount[idx];
    }
}

template<typename G, typename P>
template<template<typename> typename T>
void RAVENode<G, P>::backprop(double outcome, T<RAVENode<G, P>>* const table, unsigned leafDepth){
    auto it = game->getTakenMoves().rbegin();
    std::array<std::vector<unsigned>, 2> takenMoves;
    for(unsigned player = 0; player < 2; ++player){
        takenMoves[player] = std::vector<unsigned>{};
        takenMoves[player].reserve(game->getMaxDepth());
    }
    // go up to leaf and gather played moves
    while(game->getCurrentDepth() != leafDepth){
        auto move = *it;
        takenMoves[move.getPlayer()].push_back(game->toMoveIdx(move.getPiece(), move.getPos()));
        --it;
        // update because of depth
        game->undo();
    }
    // backprop
    RAVENode<G, P>* current = this;
    RAVENode<G, P>* currParent = table->backward();
    while(currParent){
        // action value is updated with the next player
        current->updateRAVE(outcome, takenMoves);
        // update because of available pieces
        game->undo();
        
        // state value is updated with parent's player
        current->updateMC(outcome+game->getNextPlayer()*(1.0-2.0*outcome));
        auto move = *it;
        takenMoves[move.getPlayer()].push_back(game->toMoveIdx(move.getPiece(), move.getPos()));
        --it;
        current = currParent;
        currParent = table->backward();
    }
    // root
    ++(current->mcCount);
    current->updateRAVE(outcome, takenMoves);
}

template<typename G, typename P>
double RAVENode<G, P>::getStateScore() const {
    return mcMean;
}

template<typename G, typename P>
double RAVENode<G, P>::getVisitCount() const {
    return mcCount;
}

#endif // RAVENODE_H
