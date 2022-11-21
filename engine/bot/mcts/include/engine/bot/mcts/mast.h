#ifndef MAST_H
#define MAST_H

#include <vector>
#include <array>
#include <tuple>
#include <math.h>
#include <random>

/**********************************************************************************
 * Move-Average Sampling Technique (MAST) Playout Policy                          *
 *                                                                                *
 * Search depth is added as a feature to moves to build the policy                *
 **********************************************************************************/

template<typename G>
class MAST
{
public:
    MAST(G& game, double temp=5, double w=0.98);
    ~MAST()=default;
    MAST(const MAST&)=delete;
    MAST& operator=(const MAST&)=delete;
    double simulate();
    std::tuple<unsigned int, unsigned int> select();
    void updateRoot();

    std::array<std::vector<double>, G::PIECENUM> getScores(typename G::Player player) const;

protected:
    struct Move{
        typename G::Player player;
        unsigned int moveIdx;
        Move(typename G::Player player, unsigned int moveIdx): player{player}, moveIdx{moveIdx} {}
    };

    inline void update(double outcome);

    // [depth][player][piece][pieceMoveIdx] -> score
    std::vector<std::array<std::array<std::vector<double>, G::PIECENUM>, 2>> scores;
    double w;
    double temp;
    G& game;

    // preallocate space for random sampling upfront by using the number of maximum legal moves
    std::vector<double> probs;
    std::vector<unsigned> idxMap;
    // we only need to update moves that are taken after the current root
    unsigned from;
};

template<typename G>
MAST<G>::MAST(G& game, double temp, double w):
    game(game),
    temp(temp),
    w(w),
    scores(game.getInitialPolicy()), // length of scores are expected to be greater than maxdepth
    probs(game.getMaxLegalMoveNum(), 0.5),
    idxMap(game.getMaxLegalMoveNum(), 0),
    from(0)
{}

template<typename G>
void MAST<G>::updateRoot()
{
    ++from;
}

template<typename G>
std::tuple<unsigned, unsigned> MAST<G>::select() {
    std::default_random_engine generator;
    unsigned depth = game.getCurrentDepth();
    const auto& moves = game.getValidMoves();
    unsigned idx = 0;
    for(const auto& move : moves){
        // no normalization is needed, relative volume matters
        probs[idx] = exp(scores[depth][move.player][move.piece][move.idx]/temp);
        idxMap[idx] = move.idx;
        ++idx;
    }
    auto it = probs.begin();
    advance(it, idx); // only pick from legal moves
    std::discrete_distribution<> distribution (probs.begin(), it);
    idx = distribution(generator);
    auto itM = moves.begin();
    advance(itM, idx);
    unsigned moveIdx = game.getMoveIdx(itM->piece, idxMap[idx]);
    game.update(moveIdx);
    return {moveIdx, idx};
}

template<typename G>
void MAST<G>::update(double outcome){
    auto moves = game.getTakenMoves();
    auto it = moves.begin();
    std::advance(it, from);
    while(it != moves.end()){
        // 1-outcome if black, faster then if block
        auto& move = *it;
        double val = outcome + move.player * (1.0-2.0*outcome);
        // update moving average
        unsigned depth = game.getCurrentDepth();
        scores[depth][move.player][move.piece][move.idx] = w * scores[depth][move.player][move.piece][move.idx] + (1 - w) * val;
        ++it;
    }
}

template<typename G>
double MAST<G>::simulate(){
    while(!game.end())
        // move is added during selection
        select();
    double outcome = game.getScore();
    update(outcome);
    return outcome;
}

template<typename G>
std::array<std::vector<double>, G::PIECENUM> MAST<G>::getScores(typename G::Player player) const{
    std::array<std::vector<double>, G::PIECENUM> res;
    unsigned depth = game.getCurrentDepth();
    // Available pieces can change over the course of gameplay. For chess pieces can be captured or pawns be promoted.
    // For omega and gomoku they stay the same
    // for each piece we get the win/lose statistics
    for(unsigned piece : game.getAvailablePieces())
        res[piece] = scores[depth][player][piece];
    return res;
}

#endif // MAST_H
