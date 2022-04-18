#include "mctsbot.h"
#include <stdexcept>

MCTSBot::MCTSBot(std::string game, std::string node, bool recycling, unsigned budget):
{
    if(game == "Omega"){
        typedef Omega G;
        typedef MAST<Omega> P;
        G* game = new G();
        P* policy = new P(game);
        if(recycling){
            if(node == "UCT-2"){
                typedef UCTNode<G, P> N;
                typedef RZHashTable<N> T;
                typedef StopScheduler<G, T> S;
                auto table = new T(game, policy, 20, budget);
                S* scheduler = new S(timeLeft, game, table);
                impl = new MCTS<N, G, T, P, S>(game, table, policy, scheduler);
            }
            else if(node == "MCRAVE"){
                typedef RAVENode<G, P> N;
                typedef RZHashTable<N> T;
                typedef StopScheduler<G, T> S;
                auto table = new T(game, policy, 20, budget);
                S* scheduler = new S(timeLeft, game, table);
                impl = new MCTS<N, G, T, P, S>(game, table, policy, scheduler);
            }
            else
                throw std::invalid_argument( "Invalid node string: " + node + " received" );
        }
        else{
            if(node == "UCT-2"){
                typedef UCTNode<G, P> N;
                typedef ZHashTable<N> T;
                typedef StopScheduler<G, T> S;
                auto table = new T(game, policy, 20, budget);
                S* scheduler = new S(timeLeft, game, table);
                impl = new MCTS<N, G, T, P, S>(game, table, policy, scheduler);
            }
            else if(node == "MCRAVE"){
                typedef RAVENode<G, P> N;
                typedef ZHashTable<N> T;
                typedef StopScheduler<G, T> S;
                auto table = new T(game, policy, 20, budget);
                S* scheduler = new S(timeLeft, game, table);
                impl = new MCTS<N, G, T, P, S>(game, table, policy, scheduler);
            }
            else
                throw std::invalid_argument( "Invalid node string: " + node + " received" );
        }
    }
    else
        throw std::invalid_argument( "Invalid game string: " + game + " received" );
}

void MCTSBot::updateGame(){
    impl->run();
}

void MCTSBot::update(unsigned int moveIdx){
    impl->updateRoot(moveIdx);
}
