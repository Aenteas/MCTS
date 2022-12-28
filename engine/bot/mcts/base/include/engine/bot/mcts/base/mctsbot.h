#ifndef MCTSBOT_H
#define MCTSBOT_H

#include <stdexcept>

#include "engine/bot/base/aibotbase.h"
#include "engine/game/omega/omega.h"
#include "engine/bot/scheduler/stopscheduler.h"
#include "engine/bot/mcts/hashtable/rzhashtable.h"
#include "engine/bot/mcts/hashtable/zhashtable.h"
#include "mcts.h"
#include "engine/bot/mcts/policy/mast.h"
#include "engine/bot/mcts/policy/random.h"
#include "engine/bot/mcts/exploration/uctnode.h"
#include "engine/bot/mcts/exploration/ravenode.h"

class MCTSBot: public AiBotBase
{
public:
    template<typename G>
    MCTSBot(G& game, std::string node, std::string policy, bool recycling, unsigned budget);

    ~MCTSBot() { delete impl; }

    MCTSBot(const MCTSBot&)=delete;
    MCTSBot& operator=(const MCTSBot&)=delete;
    MCTSBot(MCTSBot&&)=delete;
    MCTSBot& operator=(MCTSBot&&)=delete;

    virtual void updateByOpponent(unsigned moveIdx) override;
    void updateGame() override;
    virtual void stop() override{
        impl->stop();
    }

private:
    MCTSBase* impl;
};

// helper macro to prevent code duplication caused by runtime dependent typedefs
#define CREATE_IMPL(N, T, P)                                            \
    typedef P<G> PP;                                                    \
    typedef N<G, PP> NN;                                                \
    typedef T<NN> TT;                                                   \
    typedef StopScheduler<G, TT> S;                                     \
    PP* policy = new PP(game);                                          \
    NN::setup(&game, policy);                                           \
    TT* table;                                                          \
    if constexpr(std::is_same_v<RZHashTable<NN>, T<NN>>)                \
        table = new TT(game.getTotalValidMoveNum(), 20, budget);        \
    else                                                                \
        table = new TT(game.getTotalValidMoveNum(), 20);                \
    S* scheduler = new S(timeLeft, game, *table);                       \
    impl = new MCTS<NN, G, TT, PP, S>(game, table, policy, scheduler);  \

template<typename G>
MCTSBot::MCTSBot(G& game, std::string node, std::string policy, bool recycling, unsigned budget)
{
    try{
        if(recycling){
            if(node == "UCT-2"){
                if(policy == "random"){
                    CREATE_IMPL(UCTNode, RZHashTable, RandomPolicy)
                }
                else if(policy == "MAST"){
                    CREATE_IMPL(UCTNode, RZHashTable, MAST)
                }
                else
                    throw std::invalid_argument( "Invalid node string: " + node + " received" );
            }
            else if(node == "RAVE"){
                if(policy == "random"){
                    CREATE_IMPL(RAVENode, RZHashTable, RandomPolicy)
                }
                else if(policy == "MAST"){
                    CREATE_IMPL(RAVENode, RZHashTable, MAST)
                }
                else
                    throw std::invalid_argument( "Invalid node string: " + node + " received" );
            }
            else
                throw std::invalid_argument( "Invalid policy string: " + policy + " received" );
        }
        else{
            if(node == "UCT-2"){
                if(policy == "random"){
                    CREATE_IMPL(UCTNode, ZHashTable, RandomPolicy)
                }
                else if(policy == "MAST"){
                    CREATE_IMPL(UCTNode, ZHashTable, MAST)
                }
                else
                    throw std::invalid_argument( "Invalid node string: " + node + " received" );
            }
            else if(node == "RAVE"){
                if(policy == "random"){
                    CREATE_IMPL(RAVENode, ZHashTable, RandomPolicy)
                }
                else if(policy == "MAST"){
                    CREATE_IMPL(RAVENode, ZHashTable, MAST)
                }
                else
                    throw std::invalid_argument( "Invalid node string: " + node + " received" );
            }
            else
                throw std::invalid_argument( "Invalid policy string: " + policy + " received" );
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
