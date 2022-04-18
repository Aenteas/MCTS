#ifndef MAST_H
#define MAST_H

#include <vector>
#include <array>
#include <tuple>
#include <math.h>
#include <random>

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

    vector<double> getScores(G::Player player) const;
    // should be const specified but we want to use [] operator on scores member
    double getScore(unsigned int idx, G::Player player) const;

protected:
    struct Move{
        G::Player player;
        unsigned int moveIdx;
        Move(G::Player player, unsigned int moveIdx): player{player}, moveIdx{moveIdx} {}
    };

    inline tuple<unsigned int, unsigned int> select() const;
    inline void update(double outcome);

    array<array<double, G::MOVENUM>, 2> scores;
    double w;
    double temp;
    G& game;
};

template<typename G>
MAST<G>::MAST(G& game, double temp, double w):
    game(game),
    temp(temp),
    w(w),
    scores(game.getInitialPolicy())
{}

template<typename G>
tuple<unsigned int, unsigned int> MAST<G>::select() const{
    default_random_engine generator;
    G::Player currPlayer = game.getCurrentPlayer();
    vector<unsigned> probs;
    probs.reserve(game.getValidMoves().size());
    vector<unsigned> idxMap;
    idxMap.reserve(game.validMoves.size());
    for(unsigned int moveIdx : game.getValidMoves()){
        idxMap.push_back(moveIdx);
        // no normalization is needed, relative volume matters
        probs.push_back(exp(scores[currPlayer][moveIdx]/temp) + 1e-8);
    }

    discrete_distribution<> distribution (probs.begin(), probs.end());
    unsigned int idx = distribution(generator);
    return {idxMap[idx], idx};
}
// getMoveIdx
// takenMoves
template<typename G>
void MAST<G>::update(double outcome){
    for(const G::Move& move : game.takenMoves()){
        // 1-outcome if black, faster then if block
        double val = outcome + move.player * (1.0-2.0*outcome);
        // update moving average
        scores[move.player][game.getMoveIdx(move.idx)] = w * scores[move.player][game.getMoveIdx(move.idx)] + (1 - w) * val;
    }
}

template<typename G>
double MAST::simulate(unsigned int idx, G::Player player){
    double outcome;
    while(!game.end()){
        // move is added during selection
        auto [moveIdx, _] = select();
        game.update(moveIdx);
    }
    update(outcome);
    return outcome;
}

template<typename G>
vector<double> MAST::getScores(G::Player player) const{
    return scores[player];
}

template<typename G>
double MAST::getScore(unsigned int idx, G::Player player) const{
    return scores[player][idx];
}

#endif // MAST_H
