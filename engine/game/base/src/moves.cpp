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
    // produce random order, but keep the same order among instances to prevent inconsistencies in assign function
    // using a static sharedMoves object
    if(sharedMoves.empty() || sharedMoves.size() != cellNum) // check if different board configuration was played before
    {
        moves.reserve(cellNum);
        for(unsigned i=0; i<cellNum; ++i)
            moves.push_back({i});
        random_shuffle(moves.begin(), moves.end());
        sharedMoves = moves;
    }
    else
    {
        moves = sharedMoves;
    }
    lookup.reserve(cellNum);
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
        moves[i].piece = 2;
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

    // but we potentially copy initial game state
    firstTaken = other.firstTaken ? lookup[other.firstTaken->pos] : nullptr;
    lastTaken = other.lastTaken ? lookup[other.lastTaken->pos] : nullptr;

    numTaken = other.numTaken;
    numEmpty = other.numEmpty;
    pieceCell.piece = other.pieceCell.piece;
}

void Moves::add(unsigned player, unsigned piece, unsigned pos){
    update(pos, &firstEmpty, &lastEmpty, &firstTaken, &lastTaken);
    --numEmpty;
    ++numTaken;
    lookup[pos]->player = player;
    lookup[pos]->piece = piece;
}

void Moves::remove(unsigned pos){
    update(pos, &firstTaken, &lastTaken, &firstEmpty, &lastEmpty);
    ++numEmpty;
    --numTaken;
    lookup[pos]->piece = 2; // make it empty
}

void Moves::updateNextPiece(unsigned piece){
    pieceCell.piece = piece;
}

// plus indirection to change the pointers
void Moves::update(
    unsigned pos, 
    Move** fromFirst, 
    Move** fromLast, 
    Move** toFirst, 
    Move** toLast){
    // remove from source move group

    if(lookup[pos]->prev) // skip from source move pointer chain
        lookup[pos]->prev->next = lookup[pos]->next;
    else // shift head
        *fromFirst = (*fromFirst)->next;
    if(lookup[pos]->next) // if it is not the last item we set the next item
        lookup[pos]->next->prev = lookup[pos]->prev;
    else // shift tail
        *fromLast = (*fromLast)->prev;

    // add to target move group
    if(*toFirst){
        (*toLast)->next = lookup[pos];
        lookup[pos]->next = nullptr;
        lookup[pos]->prev = *toLast;
        *toLast = lookup[pos];
    }
    else{
        *toFirst = lookup[pos];
        *toLast = lookup[pos];
        lookup[pos]->next = nullptr;
        lookup[pos]->prev = nullptr;
    }
}

Moves::Iterator& Moves::validMoves()
{
    pieceCellpp = &pieceCellp;
    size = numEmpty; 
    first = firstEmpty; 
    last = lastEmpty; 
    return iterator; 
};

Moves::Iterator& Moves::takenMoves()
{
    pieceCellpp = &(it); // piece will be the one placed on the actual cell iterated
    size = numTaken; 
    first = firstTaken; 
    last = lastTaken;
    return iterator; 
};