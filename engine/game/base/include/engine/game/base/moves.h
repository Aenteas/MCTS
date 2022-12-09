#ifndef MOVES_H
#define MOVES_H

#include <vector>

/**************************************************************************************
 * Iterator interface to access both valid and taken moves using freelist             *
 * implementation                                                                     *  
 * - Can be used for games where in each turn only one type of piece can be placed    *
 * on any of the empty cells on the board. Example games are: gomoku, hex, omega, go. *
 * - Provides random order access and updates in O(1)                                 *
 * - Do not use multiple iterators simultaenously                                     *
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
        using value_type = const Iterator&;
        using pointer = Move*;
        using reference = Move&;
        using iterator_category = std::input_iterator_tag;

        Iterator(Moves& parent) : parent{parent} {}

        Iterator(const Iterator& other): parent(other.parent) {}

        // only used when iterating through taken moves
        unsigned getPlayer() const { return parent.it->player; }
        // for taken moves we use the piece from the cell, otherwise the current piece to place
        // by adding plus indirection there is no need to update every valid moves with the current piece to place
        unsigned getPiece() const { return (*(parent.pieceCellpp))->piece; }
        unsigned getPos() const { return parent.it->pos; }

        bool operator!=(const Move* left) const { return parent.it != left; }

        value_type operator*() const { return *this; }

        // no post increment/decrement operators
        Iterator& operator++() { 
            parent.it=parent.it->next; 
            return *this; 
        }
        Iterator& operator--() { 
            parent.it=parent.it->prev; 
            return *this; 
        }

        Iterator& begin() { 
            parent.it = parent.first; 
            return *this; 
        }
        
        Iterator& rbegin() { 
            parent.it = parent.last; 
            return *this; 
        }

        Move* end() const { 
            return nullptr; 
        }

        // iterator can be used in simple while loop
        operator bool() const{ return parent.it != nullptr; }

        unsigned size() const { return parent.size; };

    private:
        Moves& parent;
    };

    Iterator& validMoves();
    Iterator& takenMoves();

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
    inline static std::vector<Move> sharedMoves = {};
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
    Move* it;
};

#endif // MOVES_H