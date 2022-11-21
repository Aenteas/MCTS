#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include <array>

#include<list>
#include <stack>
#include <map>

/**********************************************************************************
 * Omega game engine                                                              *
 * Any 2-player board game should implement a minimal public interface with the   *
 * following functions and members:                                               *
 *                                                                                *
 * enum Player{PLAYER1, PLAYER2, DRAW};                                           *
 * enum Piece{PIECE1, PIECE2, ...};                                               *
 *                                                                                *
 * struct Move{                                                                   *
 *     Player player;                                                             *
 *     Piece piece;                                                               *
 *     unsigned idx;                                                              *
 * };                                                                             *
 * void update(unsigned moveIdx);                                                 *
 * void undo();                                                                   *
 * Player getCurrentPlayer() const;                                               *
 * Player getPreviousPlayer() const;                                              *
 * Player getLeader();                                                            *
 *                                                                                *
 * double getScore();                                                             *
 * array<double, 2> getPlayerScores() const;                                      *
 * Move getLastMove() const;                                                      *
 * unsigned getLastMoveIdx() const;                                               *
 * vector<unsigned> getValidMoveIdxs() const;                                     *
 * vector<Move> getValidMoves() const;                                            *
 * unsigned getPieceMoveIdx(unsigned moveIdx) const;                              *
 * unsigned getRandomMove() const;                                                *
 * unsigned getNumExpectedMoves() const;                                          *
 * unsigned getMaxNumMoves() const;                                               *
 * unsigned getMoveNum() const;                                                   *
 * vector<Piece> getAvailablePieces() const;                                      *
 * unsigned getCurrentDepth() const;                                              *
 * bool end() const;                                                              *
 * array<vector<double>, 2> getInitialPolicy();                                   *
 * static constexpr unsigned PIECENUM = 2;                                        *
 **********************************************************************************/

using namespace std;

struct Ax{
    int q, r;
    Ax(int q, int r): q{q}, r{r} {}
};

struct Cell;
struct Group;

class Omega
{
public:
    Omega(unsigned boardSize);

    Omega& operator=(const Omega&)=delete;
    Omega(const Omega&)=delete;
    Omega& operator=(Omega&&)=delete;
    Omega(Omega&&)=delete;

    // used for indexing, do not change the numbers
    enum Piece{WHITEPIECE=0, BLACKPIECE};
    enum Player{WHITE=0, BLACK, DRAW};

    struct Move{
        Player player;
        Piece piece;
        unsigned idx;
        Move(Player player, Piece piece, unsigned idx):
            player(player),
            piece(piece),
            idx(idx) {}
        Move(const Move&)=default;
    };

    // ---- available moves ----
    class ValidMoves
    {
        // container class for storing the available cells
    public:
        ValidMoves(unsigned cellNum);
        unsigned prevCellIdx() const;
        void remove(unsigned cellIdx);
        unsigned getRandomMove() const;
        void undo();
        unsigned size() const;

        struct Cell{
            unsigned idx;
            Cell* next;
            Cell* prev;
            Cell(unsigned idx);
        };

        struct Iterator
        {
            using iterator_category = std::forward_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = unsigned;
            using pointer           = Cell*;
            using reference         = unsigned&;

            Iterator(const pointer ptr, const ValidMoves* parent) : m_ptr(ptr), parent{parent} {}

            value_type operator*() const { return (*m_ptr).idx + parent->cellNum * parent->color; }
            const pointer operator->() const { return m_ptr; }
            const Iterator& operator++() const { m_ptr=m_ptr->next; return *this; }
            const Iterator operator++(int) const { Iterator tmp = *this; ++(*this); return tmp; }
            friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; }
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; }

        private:
            mutable pointer m_ptr;
            const ValidMoves* parent;
        };

        const Iterator begin() const { return Iterator(first, this); }
        const Iterator end() const { return Iterator(nullptr, this); }

    public:
        // cells for forward iterating in randomly ordered cells
        vector<Cell> cells;
        // lookup for instant accessing items from cells
        vector<Cell*> lookup;
        Cell* first;
        vector<unsigned> takenCells;
        unsigned mSize;
        unsigned cellNum;
        unsigned color;
    };

    // ---- updates ----
    void update(unsigned moveIdx);
    void undo();

    // ---- queries ----
    // simulation and expansion works with a default policy
    // we compute an initial policy to improve performance
    vector<array<array<vector<double>, 2>, 2>> getInitialPolicy();

    Player getCurrentPlayer() const;
    Player getPreviousPlayer() const;
    Player getLeader() const;
    Piece getCurrentPiece() const;

    // outcome in terminal state: 1 for WHITE, 0 for BLACK, 0.5 for draw
    double getScore() const;
    // 2 player-game
    array<double, 2> getPlayerScores() const;

    // Move is player-pieceid-pieceMoveIdx
    // it can be represented by a simple unsigned number - moveIdx
    Move getLastMove() const;
    unsigned getLastPieceMoveIdx() const;
    unsigned getLastMoveIdx() const;
    Omega::ValidMoves getValidMoveIdxs() const;
    vector<Move> getValidMoves() const;
    vector<Move> getTakenMoves() const;
    // moveIdx -> pieceMoveIdx
    unsigned getPieceMoveIdx(unsigned moveIdx) const;
    // pieceMoveIdx -> moveIdx
    unsigned getMoveIdx(Piece piece, unsigned pieceMoveIdx) const;
    unsigned getRandomMove() const;
    // expected number of moves to finish the game
    unsigned getNumExpectedMoves() const;
    // number of moves to always finish  the game
    unsigned getMaxNumMoves() const;
    // number of possible moves (maximum)
    unsigned getMoveNum() const;
    // number of possible moves (maximum)
    unsigned getMaxLegalMoveNum() const;

    vector<Piece> getAvailablePieces() const;

    unsigned getCurrentDepth() const;

    // check if we are in a terminal state
    bool end() const;

    static constexpr unsigned PIECENUM = 2;

private:
    const unsigned cellNum;
    inline unsigned computeCellNum(unsigned boardSize) const;

    // ---- initialization ----
    void initCells();
    void setNeighbours(Cell& cell);

    // ---- forward update ----
    void mergeGroups(Cell& cell);
    void updateColors();
    void updateNeighbourBitMaps(Cell& cell);
    void mergeNeighbourBitMaps(Cell& cell);

    // ---- backward update ----
    void decomposeGroup(Cell& cell);
    void undoColors();
    void undoOppBitMaps(const Cell& cell);

    // ---- bit manipulations ----
    unsigned popCnt64(uint64_t i);

    // --- inline functions for internal usage ---
    inline Cell& idxToCell(unsigned idx);
    inline Cell& axToCell(Ax ax);
    inline unsigned axToIdx(Ax ax) const;
    inline bool isValidAx(const Ax& ax);

    // ---- variables ----
    unsigned numSteps;
    unsigned freeNeighbourBitMapSize;
    const int boardSize;
    vector<vector<Cell>> cells;
    vector<Cell*> cellVec;

    // TODO
    array<double, 2> playerScores;
    size_t bitmapSize;
    // current piece (WHITE/BLACK to place)
    Piece currentPiece;
    // the player who's in turn
    Player currentPlayer;
    Player previousPlayer;
    list<unsigned> moveIdxs;
    ValidMoves validMoves;
    array<vector<Group>, 2> groups;
    unsigned depth;
};

struct Group
{
    Group(const unsigned id, const unsigned newGroupSize);
    // the number of cells in the group
    unsigned size;
    // super group id
    unsigned id;
    // keep track of the group ids that were connected to the new piece placed on the board
    map<Omega::Player, stack<list<unsigned>>> addedGroupIds;
    // array of long values of indicating free or taken neighbours per bit of groups
    stack<vector<long int>> freeNeighbourBitMaps;
};

// we use the enum Player from Omega class to represent the colors (DRAW means EMPTY)
// this way we do not need to convert one to the other
struct Cell{
    Cell(int q, int r, unsigned idx);
    list<unsigned> getNeighbourGroupIds(const vector<Group>& groups, Omega::Player color) const;
    unsigned findSuperGroup(unsigned id, const vector<Group>& groups) const;
    Omega::Player color;
    // axial coordinates of hexagons
    int q, r;
    // indices of hexagons (row-by-row from left to right from top to bottom)
    unsigned idx;
    // list of neighbour cells. There is no destructor because the pointers do not have ownership
    list<Cell*> neighbours;
    // group index
    int groupId;
};

#endif // GAMESTATE_H
