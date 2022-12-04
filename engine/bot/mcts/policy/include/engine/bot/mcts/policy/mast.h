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

protected:
    void update(double outcome);

    // [depth][player][piece][pieceMoveIdx] -> score
    std::vector<std::array<std::array<std::vector<double>, G::PIECENUM>, 2>> scores;
    double w;
    double temp;
    G& game;
    // preallocate space for random sampling upfront by using the number of maximum legal moves
    std::vector<double> probs;
    // we only need to update moves that are taken after the current root
    unsigned from;
};

template<typename G>
MAST<G>::MAST(G& game, double temp, double w):
    game(game),
    temp(temp),
    w(w),
    probs(game.getMaxValidMoveNum(), exp(0.5/temp)),
    from(0)
{
    std::array<std::vector<double>, G::PIECENUM> pieceScores; 
    pieceScores.fill(std::vector<double>(game.getMaxValidMoveNum(), 0.5));
    std::array<std::array<std::vector<double>, G::PIECENUM>, 2> playerScores;
    playerScores.fill(pieceScores);
    scores = std::vector<std::array<std::array<std::vector<double>, G::PIECENUM>, 2>>(game.getMaxTurnNum(), playerScores);
}

template<typename G>
void MAST<G>::updateRoot()
{
    ++from;
}

template<typename G>
std::tuple<unsigned, unsigned> MAST<G>::select() {
    std::default_random_engine generator;
    unsigned depth = game.getCurrentDepth();
    unsigned idx = 0;

    for(auto& move : game.getValidMoves()){
        auto piece = move.getPiece();
        auto pos = move.getPos();
        // no normalization is needed, relative volume matters
        probs[idx] = exp(scores[depth][game.getNextPlayer()][piece][pos]/temp);
        ++idx;
    }
    auto it = probs.begin();
    std::advance(it, idx); // only pick from legal moves
    std::discrete_distribution<> distribution (probs.begin(), it);
    idx = distribution(generator);
    auto& itSelected = game.getValidMoves().begin();
    std::advance(itSelected, idx);

    unsigned moveIdx = game.toMoveIdx(itSelected.getPiece(), itSelected.getPos());
    game.select(moveIdx);
    return {moveIdx, idx};
}

template<typename G>
void MAST<G>::update(double outcome){
    auto& moves = game.getTakenMoves().begin();
    std::advance(moves, from);
    unsigned depth = from;
    while(moves){
        // 1-outcome if black, faster then if block
        auto player = moves.getPlayer();
        auto piece = moves.getPiece();
        auto pos = moves.getPos();
        double val = outcome + player * (1.0-2.0*outcome);
        // update moving average
        scores[depth][player][piece][pos] = w * scores[depth][player][piece][pos] + (1 - w) * val;
        ++depth;
        ++moves;
    }
}

template<typename G>
double MAST<G>::simulate(){
    while(!game.end())
        // move is added during selection
        select();
    double outcome = game.outcome();
    update(outcome);
    return outcome;
}

#endif // MAST_H
