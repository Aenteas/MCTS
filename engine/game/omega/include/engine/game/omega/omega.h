#ifndef OMEGA_H
#define OMEGA_H

#include "engine/game/base/moves.h"
#include "engine/game/base/game.h"

#include <vector>
#include <array>

class Omega: public Game<Omega>
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

    Omega(unsigned boardSize);

    void assign(const Omega&); // assigment to update with root state after search is finished. It is a lightweight
    // version of the correct assignment operator
    Omega& operator=(const Omega&)=delete;
    Omega(const Omega&)=delete;
    Omega& operator=(Omega&&)=delete;
    Omega(Omega&&)=delete;

    // ---- updates ----
    void select(unsigned moveIdx);
    void undo();

    // ---- conversions ----

    // Move is piece-pos, it can be represented by a simple unsigned number - moveIdx
    // in chess pos encodes from-to, in other games like go it is simply the position of the piece
    unsigned toPos(unsigned moveIdx) const;
    unsigned toPiece(unsigned moveIdx) const;
    // piece,pos -> moveIdx
    unsigned toMoveIdx(unsigned piece, unsigned pos) const;

    // ---- queries ----

    unsigned getLastPlayer() const;

    // outcome in terminal state: 1 for WHITE, 0 for BLACK, 0.5 for draw
    const std::array<double, 2>& scores();

    const Moves::Iterator& getValidMoves();
    const Moves::Iterator& getTakenMoves();

    const Moves::Iterator& getLastMove();
    unsigned getLastMoveIdx();

    std::vector<unsigned> getAvailablePieces() const;

    // total number of valid moves
    unsigned getTotalValidMoveNum() const;
    // maximum number of valid moves that can be played in a turn
    unsigned getMaxValidMoveNum() const;
    // maximum number of turns (for both players)
    unsigned getMaxTurnNum() const;
    // maximum number of turns for a player
    unsigned getMaxPlayerTurnNum() const;

    bool end() const;

    static constexpr unsigned PIECENUM = 2;
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
};

#endif // OMEGA_H