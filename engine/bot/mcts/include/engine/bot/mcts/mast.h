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

using namespace std;

template<typename G>
class MAST
{
public:
    MAST(G& game, double temp=5, double w=0.98);
    ~MAST()=default;
    MAST(const MAST&)=delete;
    MAST& operator=(const MAST&)=delete;
    double simulate();
    tuple<unsigned int, unsigned int> select() const;
    void updateRoot();

    array<vector<double>, G::PIECENUM> getScores(typename G::Player player) const;
    double getScore(const typename G::Move& move) const;

protected:
    struct Move{
        typename G::Player player;
        unsigned int moveIdx;
        Move(typename G::Player player, unsigned int moveIdx): player{player}, moveIdx{moveIdx} {}
    };

    inline void update(double outcome);

    // it could be that for a game player can not be derived from depth
    // [depth][player][piece][pieceMoveIdx]
    vector<array<array<vector<double>, G::PIECENUM>, 2>> scores;
    double w;
    double temp;
    G& game;
    // we only need to update moves that are taken after the current root
    unsigned from;
};

template<typename G>
MAST<G>::MAST(G& game, double temp, double w):
    game(game),
    temp(temp),
    w(w),
    scores(game.getInitialPolicy()), // length of scores are expected to be greater than maxdepth
    from(0)
{}

template<typename G>
void MAST<G>::updateRoot()
{
    ++from;
}

template<typename G>
tuple<unsigned, unsigned> MAST<G>::select() const{
    default_random_engine generator;
    vector<double> probs;
    vector<unsigned> idxMap;
    probs.reserve(game.getValidMoves().size());
    idxMap.reserve(game.getValidMoves().size());
    unsigned depth = game.getCurrentDepth();
    const auto& moves = game.getValidMoves();
    for(const auto& move : moves){
        // no normalization is needed, relative volume matters
        probs.push_back(exp(scores[depth][move.player][move.piece][move.idx]/temp) + 1e-8);
        // probs.push_back(1.0);
        idxMap.push_back(move.idx);
    }

    discrete_distribution<> distribution (probs.begin(), probs.end());
    unsigned idx = distribution(generator);
    auto it = moves.begin();
    advance(it, idx);
    unsigned moveIdx = game.getMoveIdx(it->piece, idxMap[idx]);
    game.update(moveIdx);
    return {moveIdx, idx};
}

template<typename G>
void MAST<G>::update(double outcome){
    for(const typename G::Move& move : game.getTakenMoves(from)){
        // 1-outcome if black, faster then if block
        double val = outcome + move.player * (1.0-2.0*outcome);
        // update moving average
        unsigned depth = game.getCurrentDepth();
        scores[depth][move.player][move.piece][move.idx] = w * scores[depth][move.player][move.piece][move.idx] + (1 - w) * val;
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
array<vector<double>, G::PIECENUM> MAST<G>::getScores(typename G::Player player) const{
    array<vector<double>, G::PIECENUM> res;
    unsigned depth = game.getCurrentDepth();
    for(unsigned piece : game.getAvailablePieces())
        res[piece] = scores[depth][player][piece];
    return res;
}

template<typename G>
double MAST<G>::getScore(const typename G::Move& move) const{
    return scores[game.getCurrentDepth()][move.player][move.piece][move.idx];
}

#endif // MAST_H
