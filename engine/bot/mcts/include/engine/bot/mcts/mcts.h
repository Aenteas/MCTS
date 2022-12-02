#ifndef MCTS_H
#define MCTS_H

#include "node.h"
#include <stdexcept>
#include <iostream>

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
    MCTS(G& game, Omega2& game2, T* table, P* policy, S* scheduler):
        table(table),
        game(game),
        game2(game2),
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
        game2.update(moveIdx);
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
        std::cout << "num:" << scheduler->num << std::endl; 
        unsigned rootPlayer = game.getNextPlayer();
        if(rootPlayer != game2.getCurrentPlayer())
            std::cout << "DIFF nextp" << std::endl; 
        unsigned currPlayer;
        // update root by the best move
        do{
            N* bestChild = Node::selectMostVisited(table, &game, &game2);
            // depending on the hashtable implementation it could be that there was only one child explored and removed
            if(bestChild)
                root = bestChild;
            else
                root = root->expand(table);
            currPlayer = game.getNextPlayer();
            if(currPlayer != game2.getCurrentPlayer())
                std::cout << "DIFF nextp2" << std::endl; 
        }while(rootPlayer == currPlayer); // one player might have multiple moves in a turn
    }

protected:
    void selection(){
        leaf = root;
        auto child = root->select(table);
        // game.end() should return false when the search depth exceeds a predefined maximum.
        // this is to prevent infinite loops in state loops
        if(game.getCurrentDepth() != game2.getCurrentDepth())
            std::cout << "DIFF depth3 bef" << std::endl; 
        if(game.end() != game2.end())
            std::cout << "DIFF end2" << std::endl; 
        while(!game.end() and child){
            leaf = child;
            child = child->select(table);
        }
        selectionDepth = game.getCurrentDepth();
        if(game.getCurrentDepth() != game2.getCurrentDepth())
            std::cout << "DIFF depth3" << std::endl; 
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
    Omega2& game2;
    P* const policy;
    S* const scheduler;
    N* root;
    N* leaf;
};

#endif // MCTS_H
