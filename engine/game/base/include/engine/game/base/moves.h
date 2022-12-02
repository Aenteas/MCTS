#ifndef MOVES_H
#define MOVES_H

#include <vector>

/**************************************************************************************
 * Iterator interface to access both valid and taken moves using freelist             *
 * implementation                                                                     *  
 * - Can be used for games where in each turn only one type of piece can be placed    *
 * on any of the empty cells on the board. Example games are: gomoku, hex, omega, go. *
 * - Provides random order access and updates in O(1)                                 * 
 **************************************************************************************/

class Moves
{
public:
    Moves(unsigned cellNum);
    void assign(const Moves& other);

    void add(unsigned player, unsigned piece, unsigned pos);
    void remove(unsigned pos);

    void updateNextPiece(unsigned piece);

    struct Move{
        unsigned player;
        unsigned piece;
        unsigned pos;

        Move* next;
        Move* prev;
        Move(unsigned pos): pos(pos), piece(0), next(nullptr), prev(nullptr) {}
    };

    // nested iterator interface to hide the interface of the Moves class
    struct Iterator
    {
        friend class Moves;

        using difference_type = std::ptrdiff_t;
        using value_type = Move;
        using pointer = Move*;
        using reference = Move&;
        using iterator_category = std::input_iterator_tag;

        Iterator(const Moves& parent) : it(nullptr), parent{parent} {}

        // only used when iterating through taken moves
        unsigned getPlayer() const { return it->player; }
        // for taken moves we use the piece from the cell, otherwise the current piece to place
        // by adding plus indirection there is no need to update every valid moves with the current piece to place
        unsigned getPiece() const { return (*(parent.pieceCellpp))->piece; }
        unsigned getPos() const { return it->pos; }

        // no post increment/decrement operators
        const Iterator& operator++() const { 
            it=it->next; 
            return *this; 
        }
        const Iterator& operator--() const { 
            it=it->prev; 
            return *this; 
        }

        // returning constant references on the same iterator instance. Be aware when using with multithreading
        const Iterator& cbegin() const { 
            it = parent.first; 
            return *this; 
        }
        const Iterator& crbegin() const { 
            it = parent.last; 
            return *this; 
        }

        // iterator can be used in simple while loop
        operator bool() const{ return it != nullptr; }

        unsigned size() const { return parent.size; };

    private:
        mutable pointer it;
        const Moves& parent;
    };

    const Iterator& validMoves();
    const Iterator& takenMoves();

    // item can not be modified
    const Move& operator[](unsigned idx) const { return *lookup[idx]; }

private:
    void update(
        unsigned pos, 
        Move** fromFirst, 
        Move** fromLast, 
        Move** toFirst, 
        Move** toLast);
    // iterating in randomly ordered moves
    std::vector<Move> moves;
    // lookup with O(1) access to items from moves
    std::vector<Move*> lookup;
    Move* first;
    Move* last;
    Move* firstEmpty;
    Move* lastEmpty;
    Move* firstTaken;
    Move* lastTaken;

    // optimizing piece queries
    Move pieceCell; // holds a reference to the current piece to place
    Move* pieceCellp;
    Move** pieceCellpp;

    unsigned size;
    unsigned numTaken;
    unsigned numEmpty;
    const unsigned cellNum;
    Iterator iterator;
};

#endif // MOVES_H