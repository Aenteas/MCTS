#ifndef MAST_H
#define MAST_H

#include <vector>
#include <array>
#include <tuple>
#include <math.h>
#include <random>
#include <iostream>

/**********************************************************************************
 * Move-Average Sampling Technique (MAST) Playout Policy                          *
 *                                                                                *
 * Search depth is added as a feature to moves to build the policy                *
 **********************************************************************************/

template<typename G>
class MAST
{
public:
    MAST(G& game, Omega2& game2, double temp=5, double w=0.98);
    ~MAST()=default;
    MAST(const MAST&)=delete;
    MAST& operator=(const MAST&)=delete;
    double simulate();
    std::tuple<unsigned int, unsigned int> select();
    void updateRoot();

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
    Omega2& game2;
    // preallocate space for random sampling upfront by using the number of maximum legal moves
    std::vector<double> probs;
    std::vector<unsigned> idxMap;
    // we only need to update moves that are taken after the current root
    unsigned from;
};

template<typename G>
MAST<G>::MAST(G& game, Omega2& game2, double temp, double w):
    game(game),
    game2(game2),
    temp(temp),
    w(w),
    probs(game.getMaxLegalMoveNum(), 0.5),
    idxMap(game.getMaxLegalMoveNum(), 0),
    from(0)
{
    std::array<std::vector<double>, G::PIECENUM> pieceScores; 
    pieceScores.fill(std::vector<double>(game2.getMaxValidMoveNum(), 0.5));
    std::array<std::array<std::vector<double>, G::PIECENUM>, 2> playerScores;
    playerScores.fill(pieceScores);
    scores = std::vector<std::array<std::array<std::vector<double>, G::PIECENUM>, 2>>(game2.getMaxTurnNum(), playerScores);
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
    if(depth != game2.getCurrentDepth())
        std::cout << "DIFF depth" << std::endl;
    const auto& moves = game.getValidMoves();
    const auto& mms = game2.getValidMoves().cbegin();
    unsigned idx = 0;
    if(game.getCurrentPlayer() != game2.getNextPlayer())
        std::cout << "DIFF player mast" << std::endl;
    for(const auto& move : moves){
        if(mms.getPiece() != move.piece || mms.getPos() != move.idx)
            std::cout << "DIFF move" << std::endl;
        // no normalization is needed, relative volume matters
        probs[idx] = exp(scores[depth][game.getCurrentPlayer()][move.piece][move.idx]/temp);
        idxMap[idx] = move.idx;
        ++idx;
        ++mms;
    }
    auto it = probs.begin();
    std::advance(it, idx); // only pick from legal moves
    std::discrete_distribution<> distribution (probs.begin(), it);
    idx = distribution(generator);
    auto itM = moves.begin();
    std::advance(itM, idx);
    const auto& itMM = game2.getValidMoves().cbegin();
    std::advance(itMM, idx);
    if(itM->piece != itMM.getPiece())
        std::cout << "DIFF piece" << std::endl;
    unsigned moveIdx = game.getMoveIdx(itM->piece, idxMap[idx]);
    if(moveIdx != game2.toMoveIdx(itM->piece, idxMap[idx]))
        std::cout << "DIFF conv" << std::endl;
    game.update(moveIdx);
    game2.update(moveIdx);
    return {moveIdx, idx};
}

template<typename G>
void MAST<G>::update(double outcome){
    auto moves = game.getTakenMoves();
    const auto& moves2 = game2.getTakenMoves().cbegin();
    auto it = moves.begin();
    std::advance(it, from);
    std::advance(moves2, from);
    unsigned depth = from;
    // std::cout << "simupdates:" << std::endl;
    while(it != moves.end()){
        // 1-outcome if black, faster then if block
        auto& move = *it;
        if(moves2.getPiece() != move.piece || moves2.getPlayer() != move.player || moves2.getPos() != move.idx)
            std::cout << "DIFF move2" << std::endl;
        double val = outcome + move.player * (1.0-2.0*outcome);
        // std::cout << "val:" << val << " depth: "<< depth << std::endl;
        // update moving average
        scores[depth][move.player][move.piece][move.idx] = w * scores[depth][move.player][move.piece][move.idx] + (1 - w) * val;
        ++it;
        ++depth;
        ++moves2;
    }
}

template<typename G>
double MAST<G>::simulate(){
    if(game.end() != game2.end())
        std::cout << "DIFF end" << std::endl;
    while(!game.end())
        // move is added during selection
        select();
    double outcome = game.getScore();
    auto[s21, s22] = game2.scores();
    auto[s1, s2] = game.getPlayerScores();
    if(s1 != s21 || s2 != s22)
        std::cout << "DIFF scores" << std::endl;
    // else{
    //     std::cout << "s1: " << s1 << std::endl;
    //     std::cout << "s2: " << s2 << std::endl;
    // }

    if(outcome != game2.outcome())
        std::cout << "DIFF outcome" << std::endl;
    update(outcome);
    return outcome;
}

#endif // MAST_H
