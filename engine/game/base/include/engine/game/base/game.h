#ifndef GAME_H
#define GAME_H

/***********************************************************************************
 * Base class for games providing a shared root state that can be used to          *
 * periodically update the derived instance during MCTS search                     *             *
 * Plus it provides some functionalities that are universal for 2 player board     *
 * games.                                                                          *
 * Derive from Game< *your own game class* > and forward the constructor arguments *
 * to its constructor.                                                             *
 * Make sure to delete all the instances to create a new shared state:             *
 * game = make_shared<Omega>(boardSize); could not change the shared state         *
 ***********************************************************************************/

template<typename G>
class Game
{
    // this ensures G to be derived from Game and prevents wrong usage that could cause undefined behavior (static cast in update function)
    friend G;

    template<typename... Args>
    Game(Args&&... args);

public:
    // set derived instance to root state
    void selectRoot();
    // update both game and root by new move. Used to update by opponent moves or by the best move found
    // after search
    void update(unsigned idx);

    double outcome();

    unsigned getNextPlayer() const;

    unsigned getCurrentDepth() const;

    Game(const Game& game)=delete;
    Game(Game&& game)=delete;
    Game& operator=(const Game& game)=delete;
    Game& operator=(Game&& game)=delete;

    ~Game();

protected:
    // instance holding the root game state for the derived instance
    inline static G* root = nullptr;

    unsigned depth;
    unsigned nextPlayer;
private:
    inline static unsigned counter = 0;
};

template<typename G>
Game<G>::~Game()
{
    --counter;
    // shared state is only deleted when there is no instance left to prevent undefined behaviour
    // game = make_shared<Omega>(boardSize); would cause this
    if(root && counter == 0)
    {
        G* temp = root; // prevents double free
        root = nullptr;
        ++counter;
        delete temp;
    }
}

template<typename G>
template<typename... Args>
Game<G>::Game(Args&&... args):
    depth(0),
    nextPlayer(0)
{
    ++counter;
    if(!root)
    {
        root = static_cast<G*>(this); // prevents infinite recursion
        --counter;
        root = new G(std::forward<Args>(args)...);
    }
}

template<typename G>
void Game<G>::selectRoot(){
    static_cast<G&>(*this).assign(*root);
}

template<typename G>
void Game<G>::update(unsigned moveIdx){
    static_cast<G&>(*this).select(moveIdx);
    root->select(moveIdx);
}

template<typename G>
double Game<G>::outcome()
{
    auto [whiteScore, blackScore] = static_cast<G&>(*this).scores();
    if (whiteScore > blackScore)
        return 1.0;
    else if (whiteScore < blackScore)
        return 0.0;
    else
        return 0.5; // draw
}

template<typename G>
unsigned Game<G>::getNextPlayer() const
{
    return nextPlayer;
}

template<typename G>
unsigned Game<G>::getCurrentDepth() const
{
    return depth;
}

#endif // GAME_H