#include "mctsbot.h"

#include <stdexcept>

void MCTSBot::updateGame(){
    impl->run();
}

void MCTSBot::update(unsigned int moveIdx){
    impl->updateRoot(moveIdx);
}
