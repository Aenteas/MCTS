#include "mctsbot.h"

#include <stdexcept>

void MCTSBot::updateGame(){
    impl->run();
}

void MCTSBot::updateByOpponent(unsigned int moveIdx){
    impl->updateByOpponent(moveIdx);
}
