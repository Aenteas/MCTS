#include "engine/game/base/moves.h"

#include <vector>
#include <array>

class Omega2
{
public:
    struct Ax{
        int q, r;
        Ax(int q, int r): q{q}, r{r} {}
    };

    struct Hexagon{
        Hexagon(unsigned idx): idx(idx), mark(false) {}
        // indices of hexagons (row-by-row from left to right from top to bottom)
        unsigned idx;
        bool mark; // used to indicate if note is visited or not for BFS
        std::vector<Hexagon*> neighbours;
    };

    Omega2(unsigned boardSize);

    void assign(const Omega2&); // assigment to update with root state after search is finished. It is a lightweight
    // version of the correct assignment operator
    Omega2& operator=(const Omega2&)=delete;
    Omega2(const Omega2&)=delete;
    Omega2& operator=(Omega2&&)=delete;
    Omega2(Omega2&&)=delete;

    // ---- updates ----
    void update(unsigned moveIdx);
    void undo();

    // ---- conversions ----

    // Move is piece-pos, it can be represented by a simple unsigned number - moveIdx
    // in chess pos encodes from-to, in other games like go it is simply the position of the piece
    unsigned toPos(unsigned moveIdx) const;
    unsigned toPiece(unsigned moveIdx) const;
    // piece,pos -> moveIdx
    unsigned toMoveIdx(unsigned piece, unsigned pos) const;

    // ---- queries ----

    unsigned getNextPlayer() const;
    unsigned getLastPlayer() const;

    // outcome in terminal state: 1 for WHITE, 0 for BLACK, 0.5 for draw
    const std::array<double, 2>& scores();
    double outcome();

    const Moves::Iterator& getValidMoves();
    const Moves::Iterator& getTakenMoves();

    const Moves::Iterator& getLastMove();
    double getLastMoveIdx();

    std::vector<unsigned> getAvailablePieces() const;

    // total number of valid moves
    unsigned getTotalValidMoveNum() const;
    // maximum number of valid moves that can be played in a turn
    unsigned getMaxValidMoveNum() const;

    unsigned getMaxTurnNum() const;

    unsigned getCurrentDepth() const;

    // check if we are in a terminal state
    bool end() const;
private:
    // ---- initialization ----
    void initCells();
    void setNeighbours(Hexagon& hex, const std::vector<std::vector<Hexagon*>>& board, int q, int r);

    // --- helper functions for internal usage ---
    Hexagon& idxToHex(unsigned idx);
    Hexagon* axToHexP(const std::vector<std::vector<Hexagon*>>& board, Ax ax);
    inline bool isValidAx(const Ax& ax);
    inline unsigned computeCellNum(unsigned boardSize) const;

    // ---- variables ----
    unsigned numSteps;
    const int boardSize;
    std::vector<Hexagon> hexagons;

    std::array<double, 2> playerScores;
    std::vector<Hexagon*> queue;

    unsigned nextPlayer;
    unsigned nextPiece;
    unsigned depth;
    const unsigned cellNum;
    bool mark;
    Moves moves;

    static constexpr unsigned PIECENUM = 2;
};