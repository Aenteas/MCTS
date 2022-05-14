#ifndef RANDOMBOT_H
#define RANDOMBOT_H

#include "engine/bot/base/aibotbase.h"

template<typename G>
class RandomBot : public AiBotBase
{
public:
    RandomBot(G& game);
    virtual ~RandomBot()=default;

    void update(unsigned int moveIdx) override;
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
    typename G::Player rootPlayer = game.getCurrentPlayer();
    typename G::Player currPlayer;
    do{
        game.update(game.getRandomMove());
        currPlayer = game.getCurrentPlayer();
    }while(rootPlayer == currPlayer);
}

template<typename G>
void RandomBot<G>::update(unsigned int moveIdx){
    // empty, game is expected to be updated at this point by the GUI and randombot does not have an internal representation of the game state
    // other than game
}

#endif // RANDOMBOT_H
