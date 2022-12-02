#ifndef RANDOMBOT_H
#define RANDOMBOT_H

#include "engine/bot/base/aibotbase.h"

template<typename G>
class RandomBot : public AiBotBase
{
public:
    RandomBot(G& game);
    virtual ~RandomBot()=default;

    void updateByOpponent(unsigned int moveIdx) override;
    void updateGame() override;
protected:
    G& game;
};

template<typename G>
RandomBot<G>::RandomBot(G& game):
    game(game)
{}

template<typename G>
void RandomBot<G>::updateGame(){
    unsigned rootPlayer = game.getNextPlayer();
    unsigned currPlayer;
    do{
        const auto& moves = game.getValidMoves().cbegin();
        game.update(game.toMoveIdx(moves.getPiece(), moves.getPos()));
        currPlayer = game.getNextPlayer();
        ++moves;
    }while(rootPlayer == currPlayer);
}

template<typename G>
void RandomBot<G>::updateByOpponent(unsigned int moveIdx){
    // empty, game is expected to be updated at this point by the GUI and randombot does not have an internal representation of the game state
    // other than game
}

#endif // RANDOMBOT_H
