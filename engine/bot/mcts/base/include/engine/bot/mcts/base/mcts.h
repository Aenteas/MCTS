#ifndef MCTS_H
#define MCTS_H

#include "engine/bot/mcts/exploration/node.h"
#include <stdexcept>
#include <atomic>

// Type erasure
class MCTSBase{
public:
    MCTSBase()=default;
    virtual ~MCTSBase()=default;

    MCTSBase(const MCTSBase&)=delete;
    MCTSBase& operator=(const MCTSBase&)=delete;
    MCTSBase(MCTSBase&&)=delete;
    MCTSBase& operator=(MCTSBase&&)=delete;

    virtual void run()=0;
    virtual void stop()=0;
    virtual void updateByOpponent(unsigned moveIdx)=0;
};

/**********************************************************************************
 * Fully customizable Monte Carlo tree search implementation                       *
 **********************************************************************************/

// node, game, hashtable, policy, scheduler
template<typename N, typename G, typename T, typename P, typename S>
class MCTS: public MCTSBase
{
public:
    MCTS(G& game, T* table, P* policy, S* scheduler):
        table(table),
        game(game),
        policy(policy),
        scheduler(scheduler),
        root(table->selectRoot())
    {
    }

    MCTS(const MCTS&)=delete;
    MCTS& operator=(const MCTS&)=delete;
    MCTS(const MCTS&&)=delete;
    MCTS& operator=(const MCTS&&)=delete;

    virtual ~MCTS(){
        delete table;
        delete scheduler;
        delete policy;
    }

    virtual void updateByOpponent(unsigned int moveIdx) final{
        // game state should be updated in the UI
        // for example if we reach a terminal state the UI should stop the gameplay
        root = table->updateRoot(moveIdx);
        policy->updateRoot();
    }

    virtual void run() override{
        scheduler->schedule();
        interrupt.store(false);
        bool finished = scheduler->finish();
        while(!interrupt.load() && !finished){
            game.selectRoot();
            unsigned leafDepth = selection();
            double outcome = policy->simulate();
            leaf->backprop(outcome, table, leafDepth);
            finished = scheduler->finish();
        }
        if(finished){
            game.selectRoot();
            unsigned rootPlayer = game.getNextPlayer();
            unsigned currPlayer;
            // update root by the best move
            do{
                root = Node::selectMostVisited(table, &game);
                currPlayer = game.getNextPlayer();
            }while(rootPlayer == currPlayer); // one player might have multiple moves in a turn
        }
    }

    virtual void stop() override{
        interrupt.store(true);
    }
protected:
    unsigned selection(){
        leaf = root;
        auto child = root->select(table);
        // game.end() should return false when the search depth exceeds a predefined maximum.
        // this is to prevent infinite loops in state loops
        while(!game.end() and child){
            leaf = child;
            child = child->select(table);
        }
        unsigned leafDepth = game.getCurrentDepth();
        // in an existing terminal node or can we expand?
        leaf = child ? child : leaf->expand(table);
        return leafDepth;
    }

    T* const table;
    G& game;
    P* const policy;
    S* const scheduler;
    N* root;
    N* leaf;

    std::atomic<bool> interrupt;
};

#endif // MCTS_H
