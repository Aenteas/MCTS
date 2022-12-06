#include "omega.h"

#include <algorithm>
#include <cmath>

using namespace std;

// ---- initializations ----

Omega::Omega(unsigned boardSize) :   Game<Omega>(boardSize),
                                     boardSize{boardSize},
                                     cellNum{computeCellNum(boardSize)},
                                     moves{cellNum},
                                     nextPiece(0),
                                     mark(true)
{
    // each player should have equal moves so we divide by 4
    numSteps = cellNum - cellNum % 4;

    queue.reserve(numSteps);
    // we need as much bits to be able to represent each cell on the board
    initCells();
}

void Omega::assign(const Omega &other)
{
    moves.assign(other.moves);
    numSteps = other.numSteps;
    depth = other.depth;
    nextPiece = other.nextPiece;
    nextPlayer = other.nextPlayer;
}

unsigned Omega::computeCellNum(unsigned boardSize) const
{
    unsigned numRows = 2 * boardSize - 1;
    return boardSize * numRows + (numRows - 3) / 2 * ((numRows - 3) / 2 + 1) + boardSize - 1;
}

inline bool Omega::isValidAx(const Ax &ax)
{
    return std::abs(ax.q) <= boardSize - 1 and std::abs(ax.r) <= boardSize - 1 and std::abs(ax.q + ax.r) <= boardSize - 1;
}

inline Omega::Hexagon *Omega::axToHexP(const vector<vector<Hexagon *>> &board, Ax ax)
{
    return board[ax.q + boardSize - 1][ax.q >= 0 ? ax.r + boardSize - 1 : ax.r + boardSize - 1 + ax.q];
}

void Omega::setNeighbours(Hexagon &hex, const vector<vector<Hexagon *>> &board, int q, int r)
{
    vector<Ax> neighbourAxs = {{q - 1, r + 1},
                               {q - 1, r},
                               {q, r - 1},
                               {q + 1, r - 1},
                               {q + 1, r},
                               {q, r + 1}};
    // clockwise order and consecutive cells should be neighbours of each other
    vector<unsigned> idxs;
    // top edge
    if (q == -boardSize + 1 and r > 0)
        idxs = {2, 3, 4, 5, 0, 1};
    // top right edge
    else if (r == boardSize - 1 and q > -boardSize + 1)
        idxs = {1, 2, 3, 4, 5, 0};
    // bottom right edge
    else if (r >= 0 and q > 0)
        idxs = {0, 1, 2, 3, 4, 5};
    // bottom edge
    else if (q == boardSize - 1 and r < 0)
        idxs = {5, 0, 1, 2, 3, 4};
    // bottom left edge
    else if (r == -boardSize + 1 and q > 0)
        idxs = {4, 5, 0, 1, 2, 3};
    // top left edge and interior areas
    else
        idxs = {3, 4, 5, 0, 1, 2};

    hex.neighbours.reserve(6);
    for (unsigned idx : idxs)
    {
        Ax ax = neighbourAxs[idx];
        if (isValidAx(ax))
            hex.neighbours.push_back(axToHexP(board, ax));
    }
}

void Omega::initCells()
{
    vector<vector<Hexagon *>> board;
    board.reserve(2 * boardSize - 1);
    hexagons.reserve(cellNum);
    vector<Ax> axes;
    axes.reserve(cellNum);
    for (unsigned i = 0; i < cellNum; ++i)
        hexagons.push_back({i});

    unsigned idx = 0;
    for (int q = -boardSize + 1; q < boardSize; q++)
    {
        board.push_back({});
        // we are setting raw pointers on the container while pushing back the items
        // reserving prevents the push_back operator from changing the address
        board[q + boardSize - 1].reserve(2 * boardSize - 1 - std::abs(q));
        for (int r = -boardSize + 1; r < boardSize; r++)
        {
            if (isValidAx({q, r}))
            {
                axes.push_back({q, r});
                board[q + boardSize - 1].push_back(&hexagons[idx]);
                ++idx;
            }
        }
    }
    for (unsigned int i = 0; i < cellNum; ++i)
        setNeighbours(hexagons[i], board, axes[i].q, axes[i].r);
}

// ----- updates ------

void Omega::select(unsigned moveIdx)
{
    unsigned pos = toPos(moveIdx);
    moves.add(nextPlayer, nextPiece, pos);
    hexagons[pos].mark = mark; // update with new mark
    --numSteps;
    ++depth;
    nextPiece = depth & 1u;          // modulo 2 -> every turn switch piece color
    nextPlayer = (depth & 2u) >> 1u; // every second turn switch players
    moves.updateNextPiece(nextPiece);
}

void Omega::undo()
{
    ++numSteps;
    --depth;
    nextPiece = depth & 1u;  // modulo 2 -> every turn switch piece color
    nextPlayer = (depth & 2u) >> 1u; // every second turn switch players
    // no need to update moves as they only used at the terminal state when calculating the outcome
}

// ----- queries ------

inline Omega::Hexagon& Omega::idxToHex(unsigned idx)
{
    return hexagons[idx];
}

const array<double, 2>& Omega::scores()
{
    playerScores[0] = 1;
    playerScores[1] = 1;
    auto takenMoves = moves.takenMoves().begin();
    unsigned start = 0;
    unsigned end = 0;
    while (takenMoves)
    {
        // BFS on each occupied hexagons to get the connected groups, then multiply their sizes together
        unsigned groupSize = 0;
        unsigned pos = takenMoves.getPos();
        unsigned piece = takenMoves.getPiece();
        if (hexagons[pos].mark == mark)
        { // visited ? taken so should not be empty
            hexagons[pos].mark = !mark;
            ++groupSize;
            queue[end] = &hexagons[pos];
            ++end;
            while (end - start > 0)
            { // not empty
                auto hex = queue[start];
                for (Hexagon *neighbour : hex->neighbours)
                {
                    if (neighbour->mark == mark && piece == moves[neighbour->idx].piece)
                    {                            // visited ?, same type of piece as seed ?
                        neighbour->mark = !mark; // set to visited
                        queue[end] = neighbour;  // add to queue
                        ++groupSize;
                        ++end;
                    }
                }
                ++start; // remove front from queue
            }
            playerScores[piece] *= groupSize;
        }
        ++takenMoves;
    }
    auto &emptyCells = moves.validMoves().begin();
    // set rest of the hexagons to visited
    while (emptyCells)
    {
        hexagons[emptyCells.getPos()].mark = !mark;
        ++emptyCells;
    }
    // negate mark so no need to update marks for every hexagon when calling score next time
    mark = !mark;
    start = 0;
    end = 0;
    return playerScores;
}

unsigned Omega::getLastPlayer() const
{
    if (nextPiece)
        return nextPlayer;
    else
        return 1 - nextPlayer;
}

Moves::Iterator& Omega::getValidMoves()
{
    return moves.validMoves();
}

Moves::Iterator& Omega::getTakenMoves()
{
    return moves.takenMoves();
}

Moves::Iterator& Omega::getLastMove()
{
    return moves.takenMoves().rbegin();
}

unsigned Omega::getLastMoveIdx()
{
    const auto& lastMove = getLastMove();
    return toMoveIdx(lastMove.getPiece(), lastMove.getPos());
}

std::vector<unsigned> Omega::getAvailablePieces() const
{
    return {nextPiece};
}

unsigned Omega::getPieceMaxNumMoves(unsigned) const
{
    return cellNum;
}

unsigned Omega::getTotalValidMoveNum() const
{
    return cellNum * 2;
}

unsigned Omega::getMaxValidMoveNum() const
{
    return cellNum;
}

unsigned Omega::getMaxTurnNum() const
{
    return numSteps;
}

unsigned Omega::getMaxPlayerTurnNum() const
{
    return (numSteps + 2) / 4;
}

bool Omega::end() const
{
    return numSteps == 0;
}

// ---- conversions ----

unsigned Omega::toPos(unsigned moveIdx) const
{
    return moveIdx % cellNum;
}

unsigned Omega::toPiece(unsigned moveIdx) const
{
    return moveIdx / cellNum;
}

unsigned Omega::toMoveIdx(unsigned piece, unsigned pos) const
{
    // for Omega color alone identifies piece type
    return pos + piece * cellNum;
}