#ifndef MCTS_H
#define MCTS_H

/**********************************************************************************
 * Monte Carlo tree search implementation                                         *
 **********************************************************************************/

// node, hashtable, game, policy, scheduler
template<typename N, typename G, typename T, typename P, typename S>
class MCTS: public MCTSBase
{
public:
    MCTS(G* game, T* table, P* policy, S* scheduler):
        table(table),
        game(game),
        policy(policy),
        scheduler(scheduler),
        root(table->selectRoot())
    {
        N::setup(game, policy);
    }

    MCTS(const MCTS&)=delete;
    MCTS& operator=(const MCTS&)=delete;
    MCTS(const MCTS&&)=delete;
    MCTS& operator=(const MCTS&&)=delete;

    virtual ~MCTS(){
        delete table;
        delete scheduler;
        delete policy;
        delete game;
    }

    // call when we update by opponents' move
    virtual void updateRoot(unsigned int moveIdx) final{
        // game is expected to be updated at this point
        root = table->updateRoot(moveIdx);
    }

    virtual void run() override{
        scheduler->schedule();
        while(!scheduler->finish()){
            selection();
            double outcome = simulation();
            backpropagation(outcome);
        }
        Color rootPlayer = game->getCurrentPlayer();
        // update root by the best move
        do{
            NodeType* bestChild = Node::selectMostVisited();
            // depending on the hashtable implementation it could be that there was only one child explored and removed
            if(bestChild)
                root = bestChild;
            else
                root = root->expand(table);
            currPlayer = game->getCurrentPlayer();
        }while(rootPlayer == currPlayer); // one player might have multiple moves in a turn
    }

protected:
    void selection(){
        leaf = root;
        child = root->select(table);
        // game->end() should return false when the search depth exceeds a predefined maximum.
        // this is to prevent infinite loops in state loops
        while(!game->end() and child){
            leaf = child;
            child = child->select(table);
        }
        // expansion, only expand non-terminal node
        if(!game->end()){
            // it returns the simulated child of the expanded node
            leaf = leaf->expand(table);
            selectionDepth = game->getCurrentDepth();
        }
    }

    double simulation(){
        double outcome = policy->simulate();
        // backward gamestate, transposition table and optionally collect additional data from simulation depending on the type of the node
        while(game->getCurrentDepth() > selectionDepth)
            N::backward();
        return outcome;
    }

    void backpropagation(double outcome){
        leaf->backprop(outcome, table);
    }

    unsigned selectionDepth;
    T* const table;
    G* game;
    P* policy;
    S* scheduler;
    N* root;
    N* leaf;
};

#endif // MCTS_H
