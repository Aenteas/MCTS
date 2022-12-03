#ifndef MCTS_H
#define MCTS_H

#include "node.h"
#include <stdexcept>

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
        while(!scheduler->finish()){
            game.selectRoot();
            selection();
            double outcome = simulation();
            backpropagation(outcome);
        }
        game.selectRoot();
        unsigned rootPlayer = game.getNextPlayer();
        unsigned currPlayer;
        // update root by the best move
        do{
            N* bestChild = Node::selectMostVisited(table, &game);
            // depending on the hashtable implementation it could be that there was only one child explored and removed
            if(bestChild)
                root = bestChild;
            else
                root = root->expand(table);
            currPlayer = game.getNextPlayer();
        }while(rootPlayer == currPlayer); // one player might have multiple moves in a turn
    }

protected:
    void selection(){
        leaf = root;
        auto child = root->select(table);
        // game.end() should return false when the search depth exceeds a predefined maximum.
        // this is to prevent infinite loops in state loops
        while(!game.end() and child){
            leaf = child;
            child = child->select(table);
        }
        selectionDepth = game.getCurrentDepth();
        leaf = child ? child : leaf->expand(table);
    }

    double simulation(){
        double outcome = policy->simulate();
        // backward gamestate, transposition table and optionally collect additional data from simulation depending on the type of the node
        leaf->setupBackProp(selectionDepth);
        return outcome;
    }

    void backpropagation(double outcome){
        leaf->backprop(outcome, table);
    }

    unsigned selectionDepth;
    T* const table;
    G& game;
    P* const policy;
    S* const scheduler;
    N* root;
    N* leaf;
};

#endif // MCTS_H
