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
        unsigned player;
        unsigned int moveIdx;
        Move(unsigned player, unsigned int moveIdx): player{player}, moveIdx{moveIdx} {}
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
    probs(game.getMaxValidMoveNum(), 0.5),
    idxMap(game.getMaxValidMoveNum(), 0),
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
    if(depth != game2.getCurrentDepth())
        std::cout << "DIFF depth" << std::endl;

    auto moves2 = game2.getValidMoves();
    auto moves2it = moves2.begin();
    const auto& moves = game.getValidMoves().cbegin();

    unsigned idx = 0;
    if(game2.getCurrentPlayer() != game.getNextPlayer())
        std::cout << "DIFF player mast" << std::endl;

    while(moves){
        auto& move = *moves2it;
        if(moves.getPiece() != move.piece || moves.getPos() != move.idx)
            std::cout << "DIFF move" << std::endl;
        
        auto piece = moves.getPiece();
        auto pos = moves.getPos();
        // no normalization is needed, relative volume matters
        probs[idx] = exp(scores[depth][game.getNextPlayer()][piece][pos]/temp);
        idxMap[idx] = pos;
        ++idx;
        ++moves;
        ++moves2it;
    }
    auto it = probs.begin();
    std::advance(it, idx); // only pick from legal moves
    std::discrete_distribution<> distribution (probs.begin(), it);
    idx = distribution(generator);
    auto M = game2.getValidMoves();
    auto itM = M.begin();
    std::advance(itM, idx);
    const auto& itMM = game.getValidMoves().cbegin();
    std::advance(itMM, idx);
    if(itM->piece != itMM.getPiece())
        std::cout << "DIFF piece" << std::endl;

    unsigned moveIdx = game.toMoveIdx(itMM.getPiece(), idxMap[idx]);
    if(moveIdx != game2.getMoveIdx(itM->piece, idxMap[idx]))
        std::cout << "DIFF conv" << std::endl;
    game.select(moveIdx);
    game2.update(moveIdx);
    return {moveIdx, idx};
}

template<typename G>
void MAST<G>::update(double outcome){
    auto moves2 = game2.getTakenMoves();
    const auto& moves = game.getTakenMoves().cbegin();
    auto it = moves2.begin();
    std::advance(it, from);
    std::advance(moves, from);
    unsigned depth = from;
    std::cout << "simupdates:" << std::endl;
    while(moves){
        // 1-outcome if black, faster then if block
        auto& move = *it;
        if(moves.getPiece() != move.piece || moves.getPlayer() != move.player || moves.getPos() != move.idx)
            std::cout << "DIFF move2" << std::endl;
        auto player = moves.getPlayer();
        auto piece = moves.getPiece();
        auto pos = moves.getPos();
        double val = outcome + player * (1.0-2.0*outcome);
        std::cout << "val:" << val << " depth: "<< depth << std::endl;
        // update moving average
        scores[depth][player][piece][pos] = w * scores[depth][player][piece][pos] + (1 - w) * val;
        ++it;
        ++depth;
        ++moves;
    }
}

template<typename G>
double MAST<G>::simulate(){
    if(game.end() != game2.end())
        std::cout << "DIFF end" << std::endl;
    while(!game.end())
        // move is added during selection
        select();
    double outcome = game.outcome();
    auto[s21, s22] = game.scores();
    auto[s1, s2] = game2.getPlayerScores();
    if(s1 != s21 || s2 != s22)
        std::cout << "DIFF scores" << std::endl;
    else{
        std::cout << "s1: " << s1 << std::endl;
        std::cout << "s2: " << s2 << std::endl;
    }

    if(outcome != game2.getScore())
        std::cout << "DIFF outcome" << std::endl;
    update(outcome);
    return outcome;
}

#endif // MAST_H
