#ifndef MCTSBOT_H
#define MCTSBOT_H

#include <stdexcept>

#include "engine/bot/base/aibotbase.h"
#include "engine/game/omega/omega.h"
#include "engine/bot/scheduler/stopscheduler.h"
#include "engine/bot/hashtable/rzhashtable.h"
#include "engine/bot/hashtable/zhashtable.h"
#include "mcts.h"
#include "mast.h"
#include "uctnode.h"

class MCTSBot: public AiBotBase
{
public:
    template<typename G>
    MCTSBot(G& game, std::string node, bool recycling, unsigned budget);

    ~MCTSBot() { delete impl; }

    MCTSBot(const MCTSBot&)=delete;
    MCTSBot& operator=(const MCTSBot&)=delete;
    MCTSBot(MCTSBot&&)=delete;
    MCTSBot& operator=(MCTSBot&&)=delete;

    virtual void updateByOpponent(unsigned moveIdx) override;
    void updateGame() override;

private:
    MCTSBase* impl;
    Omega2 game2;
};

template<typename G>
MCTSBot::MCTSBot(G& game, std::string node, bool recycling, unsigned budget): game2(5)
{
    try{
        typedef MAST<G> P;
        P* policy = new P(game, game2);
        if(recycling){
            if(node == "UCT-2"){
                typedef UCTNode<G, P> N;
                typedef RZHashTable<N> T;
                typedef StopScheduler<G, T> S;
                N::setup(&game, &game2, policy);
                auto table = new T(game.getMoveNum(), 20, budget);
                S* scheduler = new S(timeLeft, game, *table);
                impl = new MCTS<N, G, T, P, S>(game, game2, table, policy, scheduler);
            }
            else
                throw std::invalid_argument( "Invalid node string: " + node + " received" );
        }
        else{
            if(node == "UCT-2"){
                typedef UCTNode<G, P> N;
                typedef ZHashTable<N> T;
                typedef StopScheduler<G, T> S;
                N::setup(&game, &game2, policy);
                auto table = new T(game.getMoveNum(), 20);
                S* scheduler = new S(timeLeft, game, *table);
                impl = new MCTS<N, G, T, P, S>(game, game2, table, policy, scheduler);
            }
            else
                throw std::invalid_argument( "Invalid node string: " + node + " received" );
        }
    }
    catch(std::bad_alloc& e){
        throw std::runtime_error(std::string(e.what()) + ": Probably memory run out during node allocation for the transposition table. Please change the parameters.");
    }
    catch(std::invalid_argument& e){
        throw std::invalid_argument(std::string(e.what()) + ": Please change the parameters.");
    }
    catch(std::exception& e){
        throw e;
    }
    catch(...){
        throw std::runtime_error("Unknown error during the construction of MCTSBot.");
    }
}

#endif // MCTSBOT_H
