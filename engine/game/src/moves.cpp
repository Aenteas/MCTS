#include "moves.h"
#include <algorithm>

using namespace std;

Moves::Moves(unsigned cellNum):
    numTaken(0),
    numEmpty(cellNum),
    cellNum(cellNum),
    firstTaken(nullptr),
    lastTaken(nullptr),
    iterator{*this},
    pieceCell{0},
    pieceCellp(&pieceCell),
    pieceCellpp(&pieceCellp)
{
    moves.reserve(cellNum);
    lookup.reserve(cellNum);
    for(unsigned i=0; i<cellNum; ++i)
        moves.push_back({i});
    // produce random order
    random_shuffle(moves.begin(), moves.end());
    for(unsigned idx=0; idx<cellNum; ++idx){
        for(unsigned i=0; i<cellNum; ++i){
            if(moves[i].pos == idx)
                lookup.push_back(&moves[i]);
        }
    }
    // set up the chain of pointers
    firstEmpty = &moves[0];
    lastEmpty = &moves[cellNum - 1];

    moves[0].piece = 2; // empty
    moves[0].next = &moves[1];
    for(unsigned i=1; i<cellNum-1; ++i){
        moves[i].prev = &moves[i-1];
        moves[i].next = &moves[i+1];
        moves[0].piece = 2;
    }
    // cellNum should be at least 2
    moves[cellNum-1].prev = &moves[cellNum-2];
    moves[cellNum-1].piece = 2;
}

void Moves::assign(const Moves& other){
    // other should have the same board size
    for(unsigned int i = 0; i < moves.size(); ++i){
        auto prev = other.moves[i].prev;
        moves[i].prev = prev ? lookup[prev->pos] : nullptr;

        auto next = other.moves[i].next;
        moves[i].next = next ? lookup[next->pos] : nullptr;

        moves[i].piece = other.moves[i].piece;
    }

    // we do not copy game with terminal state -> firstEmpty and lastEmpty are never nullptr
    firstEmpty = lookup[other.firstEmpty->pos];
    lastEmpty = lookup[other.lastEmpty->pos];

    // we neither copy the initial state but just use the constructor to create it -> firstTaken and lastTaken are never nullptr
    firstTaken = lookup[other.firstTaken->pos];
    lastTaken = lookup[other.lastTaken->pos];

    // copy taken moves
    auto otherTaken = other.firstTaken;
    auto taken = firstTaken;
    while(otherTaken){
        taken->player = otherTaken->player;
        taken->piece = otherTaken->piece;

        otherTaken = otherTaken->next;
        taken = taken->next;
    }

    numTaken = other.numTaken;
    numEmpty = other.numEmpty;
    pieceCell.piece = other.pieceCell.piece;
}

void Moves::add(unsigned player, unsigned piece, unsigned pos){
    update(pos, firstEmpty, lastEmpty, firstTaken, lastTaken);
    --numEmpty;
    ++numTaken;
    lookup[pos]->player = player;
    lookup[pos]->piece = piece;
}

void Moves::remove(unsigned pos){
    update(pos, firstTaken, lastTaken, firstEmpty, lastEmpty);
    ++numEmpty;
    --numTaken;
    lookup[pos]->piece = 2; // make it empty
}

void Moves::updateNextPiece(unsigned piece){
    pieceCell.piece = piece;
}

void Moves::update(
    unsigned pos, 
    Cell* fromFirst, 
    Cell* fromLast, 
    Cell* toFirst, 
    Cell* toLast){
    // remove from source move group

    if(lookup[pos]->prev) // skip from source move pointer chain
        lookup[pos]->prev->next = lookup[pos]->next;
    else // shift head
        fromFirst = fromFirst->next;
    if(lookup[pos]->next) // if it is not the last item we set the next item
        lookup[pos]->next->prev = lookup[pos]->prev;
    else // shift tail
        fromLast = fromLast->prev;

    // add to target move group
    if(toFirst){
        toLast->next = lookup[pos];
        lookup[pos]->next = nullptr;
        lookup[pos]->prev = toLast;
        toLast = lookup[pos];
    }
    else{
        toFirst = lookup[pos];
        toLast = lookup[pos];
        lookup[pos]->next = nullptr;
        lookup[pos]->prev = nullptr;
    }
}

const Moves::Iterator& Moves::validMoves() 
{
    pieceCellpp = &pieceCellp;
    size = numEmpty; 
    first = firstEmpty; 
    last = lastEmpty; 
    return iterator; 
};

const Moves::Iterator& Moves::takenMoves() 
{
    pieceCellpp = iterator.it; // piece will be the one placed on the actual cell iterated
    size = numTaken; 
    first = firstTaken; 
    last = lastTaken; 
    return iterator; 
};