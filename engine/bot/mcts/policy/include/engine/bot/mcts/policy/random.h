#ifndef RANDOMP_H
#define RANDOMP_H

#include <vector>
#include <array>
#include <tuple>
#include <math.h>
#include <random>

template<typename G>
class RandomPolicy
{
public:
    RandomPolicy(G& game);
    ~RandomPolicy()=default;
    RandomPolicy(const RandomPolicy&)=delete;
    RandomPolicy& operator=(const RandomPolicy&)=delete;
    double simulate();
    std::tuple<unsigned int, unsigned int> select();
    void updateRoot() { /** empty **/ }

protected:
    G& game;
    // preallocate space for random sampling upfront by using the number of maximum legal moves
    std::vector<unsigned> probs;
};

template<typename G>
RandomPolicy<G>::RandomPolicy(G& game):
    game(game),
    probs(game.getMaxValidMoveNum(), 1)
{}

template<typename G>
std::tuple<unsigned, unsigned> RandomPolicy<G>::select() {
    std::default_random_engine generator;
    auto& moves = game.getValidMoves();
    unsigned idx = moves.size();
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
double RandomPolicy<G>::simulate(){
    while(!game.end())
        // move is added during selection
        select();
    double outcome = game.outcome();
    return outcome;
}

#endif // RANDOMP_H
