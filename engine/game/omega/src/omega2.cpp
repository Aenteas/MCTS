#include "omega2.h"

#include <algorithm>
#include <cmath>

// ---- initializations ----

Omega2::Omega2(unsigned boardSize):
    boardSize{boardSize},
    bitmapSize{sizeof(long int)*8},
    currentPiece{WHITEPIECE},
    playerScores{0,0},
    cellNum{computeCellNum(boardSize)},
    validMoves{cellNum},
    currentPlayer{Player::WHITE},
    previousPlayer{Player::WHITE},
    depth(0)
{
    // each player should have equal moves so we divide by 4
    numSteps = cellNum - cellNum%4;
    // we need as much bits to be able to represent each cell on the board
    freeNeighbourBitMapSize = (cellNum+bitmapSize-1)/bitmapSize;
    initCells();
}

inline bool Omega2::isValidAx(const Ax& ax)
{
    return std::abs(ax.q)<=boardSize-1 and std::abs(ax.r)<=boardSize-1 and std::abs(ax.q + ax.r)<=boardSize-1;
}

void Omega2::setNeighbours(Cell& cell){
    vector<Ax> neighbourAxs = {{cell.q-1, cell.r+1},
                               {cell.q-1, cell.r},
                               {cell.q, cell.r-1},
                               {cell.q+1, cell.r-1},
                               {cell.q+1, cell.r},
                               {cell.q, cell.r+1}};
    //clockwise order and consecutive cells should neighbours of each other
    vector<unsigned> idxs;
    // top edge
    if(cell.q == -boardSize+1 and cell.r > 0){
        idxs = {2,3,4,5,0,1};
    }
    // top right edge
    else if(cell.r == boardSize-1 and cell.q > -boardSize+1){
        idxs = {1,2,3,4,5,0};
    }
    // bottom right edge
    else if(cell.r >=0 and cell.q > 0){
        idxs = {0,1,2,3,4,5};
    }
    // bottom edge
    else if(cell.q == boardSize-1 and cell.r < 0){
        idxs = {5,0,1,2,3,4};
    }
    // bottom left edge
    else if(cell.r == -boardSize+1 and cell.q > 0){
        idxs = {4,5,0,1,2,3};
    }
    // top left edge and interior areas
    else{
        idxs = {3,4,5,0,1,2};
    }

    for(unsigned idx : idxs){
        Ax ax = neighbourAxs[idx];
        if(isValidAx(ax)) cell.neighbours.push_back(&axToCell(ax));
    }
}

unsigned Omega2::computeCellNum(unsigned boardSize) const{
    unsigned numRows = 2*boardSize-1;
    return boardSize*numRows+(numRows-3)/2*((numRows-3)/2+1)+boardSize-1;
}

void Omega2::initCells(){
    cells.reserve(2*boardSize-1);
    cellVec.reserve(cellNum);
    unsigned idx = 0;
    for (int q = -boardSize+1; q < boardSize; q++)
    {
        cells.push_back({});
        // we are setting raw pointers on the container while pushing back the items
        // reserving prevents the push_back operator from changing the address
        cells[q+boardSize-1].reserve(2*boardSize-1 - std::abs(q));
        for (int r = -boardSize+1; r < boardSize; r++)
        {
            if(isValidAx({q, r}))
            {
                cells[q+boardSize-1].push_back({q, r, idx});

                cellVec.push_back(&cells[q+boardSize-1][cells[q+boardSize-1].size()-1]);
                ++idx;
            }
        }
    }
    for(Cell* cell : cellVec) setNeighbours(*cell);
}

// ---- forward updates ----

void Omega2::update(unsigned moveIdx){
    moveIdxs.push_back(moveIdx);
    unsigned cellIdx = getLastPieceMoveIdx();
    validMoves.remove(cellIdx);
    Cell& cell = idxToCell(cellIdx);
    cell.color = (Player)currentPiece;
    mergeGroups(cell);
    // updateNeighbourBitMaps(cell);
    --numSteps;
    updateColors();
    ++depth;
}

void Omega2::updateColors(){
    if(currentPiece == WHITEPIECE){
        currentPiece = BLACKPIECE;
        previousPlayer = previousPlayer == Player::WHITE? Player::BLACK : Player::WHITE;
    }
    else{
        previousPlayer = currentPlayer;
        currentPlayer = currentPlayer == Player::WHITE? Player::BLACK : Player::WHITE;
        currentPiece = WHITEPIECE;
    }
}

void Omega2::mergeGroups(Cell& cell)
{
    Player color = cell.color;
    unsigned newGroupSize=1;

    //the array of neighbour super groups
    list<unsigned> addedNeighbourSuperGroupIds;
    if(groups[color].size()>0){
        addedNeighbourSuperGroupIds = cell.getNeighbourGroupIds(groups[color], color);
    }
    else{
        playerScores[color] = 1;
        goto singleGroup;
    }

    //If there is no neighbour of the same color
    if(addedNeighbourSuperGroupIds.size()==0)
    {
        singleGroup:
        // create a new group with size 1
        unsigned groupId = groups[color].size();
        cell.groupId = groupId;
        groups[color].push_back({groupId, newGroupSize});
        return;
    }

    // the first is going to be the new supergroup id
    unsigned moveGroupId = addedNeighbourSuperGroupIds.front();

    //update the groupnum of the cell
    cell.groupId = moveGroupId;

    // update neighbour groups and compute new group size
    for(unsigned nGroupId: addedNeighbourSuperGroupIds)
    {
        Group& neighbourGroup = groups[color][nGroupId];
        // add the size of the groups to the size of the new group
        newGroupSize += neighbourGroup.size;
        playerScores[color] /= neighbourGroup.size;

        // update neighbour group with new super group id
        neighbourGroup.id = moveGroupId;
    }
    // we only store the other groups
    addedNeighbourSuperGroupIds.pop_front();

    // add the list of group numbers of other groups'
    Group& group = groups[color][moveGroupId];
    group.addedGroupIds[color].push(std::move(addedNeighbourSuperGroupIds));

    group.size = newGroupSize;
    //update the new groupsize and add the addedgroups
    playerScores[color] *= newGroupSize;
}

void Omega2::updateNeighbourBitMaps(Cell& cell)
{
    // get color, group, idx of the piece that has been placed
    unsigned groupId = cell.groupId;
    Player color = cell.color;
    Player oppColor = color == WHITE?BLACK:WHITE;
    unsigned idx = cell.idx;
    Group& group = groups[color][groupId];

    // the index of the long value
    unsigned bitMapIdx=idx/bitmapSize;
    // the bit position in the long value
    unsigned bitPos=idx-bitMapIdx*bitmapSize;

    // init neighbourmap of the group
    group.freeNeighbourBitMaps.push(vector<long int>(freeNeighbourBitMapSize));

    if(group.size > 1){
        //merge the bitmaps of the old groups to the new
        mergeNeighbourBitMaps(cell);
    }

    //remove the field of the move from the map as it is taken
    group.freeNeighbourBitMaps.top()[bitMapIdx]&=~(1<<bitPos);

    list<unsigned> oppNGroupIds = cell.getNeighbourGroupIds(groups[oppColor], oppColor);

    // we keep the values for backward update
    group.addedGroupIds[oppColor].push(oppNGroupIds);
    if(oppNGroupIds.size() > 0){
        // set the bit index of the move to zero in groups of the opposite color
        for(unsigned oppNeighbourGroupId : group.addedGroupIds[oppColor].top())
            groups[oppColor][oppNeighbourGroupId].freeNeighbourBitMaps.top()[bitMapIdx]&=~(1<<bitPos);
    }

    //add the free neighbours around the cell
    // iterate over the neighbours
    for(const Cell* nCell : cell.neighbours)
    {
        if(nCell->color==Player::DRAW)
        {
            bitMapIdx=nCell->idx/bitmapSize;
            bitPos=nCell->idx-bitMapIdx*bitmapSize;
            group.freeNeighbourBitMaps.top()[bitMapIdx]|=(1<<bitPos);
        }
    }
}

void Omega2::mergeNeighbourBitMaps(Cell& cell)
{
    unsigned groupId = cell.groupId;
    Player color = cell.color;
    Group& group = groups[color][groupId];
    unsigned i;
    // we check each connected group neighbour map and use bitwise or (bit value 1 means free neighbour)
    for(unsigned nGroupId : group.addedGroupIds[color].top())
    {
        // iterate over the long values of the map
        i = 0;
        for(const auto& bMap : groups[color][nGroupId].freeNeighbourBitMaps.top()){
            // bitwise or
            group.freeNeighbourBitMaps.top()[i] |= bMap;
            ++i;
        }
    }
}

// ---- backward updates ----

void Omega2::undo()
{
    // we expect that the caller do not call when there is no taken cells
    Cell& cell = idxToCell(getLastPieceMoveIdx());

    // undoOppBitMaps(cell);
    decomposeGroup(cell);
    ++numSteps;
    undoColors();

    validMoves.undo();
    moveIdxs.pop_back();
    --depth;
}

void Omega2::undoColors(){
    if(currentPiece == WHITEPIECE){
        currentPiece = BLACKPIECE;
        currentPlayer = currentPlayer == Player::WHITE? Player::BLACK : Player::WHITE;
    }
    else
        currentPiece = WHITEPIECE;
}

void Omega2::decomposeGroup(Cell& cell)
{
    Player color = cell.color;
    cell.color = Player::DRAW;
    Player oppColor = color == Player::WHITE?Player::BLACK:Player::WHITE;
    unsigned groupId = cell.groupId;
    Group& group = groups[color][groupId];

    // no connection, just remove the group
    if(group.size==1)
    {
        groups[color].pop_back();
        return;
    }
    playerScores[color] /= group.size;

    //decrease the size with the removed field
    --group.size;

    // group was already existing, we remove the last bitmap
    // group.freeNeighbourBitMaps.pop();

    // restore component groups
    for(unsigned cGroupId : group.addedGroupIds[color].top()){
        Group& component = groups[color][cGroupId];
        playerScores[color] *= component.size;
        group.size -= component.size;
        component.id = cGroupId;
    }

    // group was already existing multiply back with the original size
    playerScores[color] *= group.size;

    group.addedGroupIds[color].pop();
    // group.addedGroupIds[oppColor].pop();
}

void Omega2::undoOppBitMaps(const Cell& cell)
{
    // get color, group, idx of the piece has been placed
    unsigned groupId = cell.groupId;
    Player color = cell.color;
    Player oppColor = color == Player::WHITE?Player::BLACK:Player::WHITE;
    unsigned idx = cell.idx;

    Group& group = groups[color][groupId];

    // the index of the long value
    unsigned bitMapIdx=idx/bitmapSize;
    // the bit position in the long value
    unsigned bitPos=idx-bitMapIdx*bitmapSize;

    //set the bit of the move on the map of each group of the opposite color
    for(unsigned oppNeighbourGroupId : group.addedGroupIds[oppColor].top())
    {
        Group& oppGroup = groups[oppColor][oppNeighbourGroupId];
        oppGroup.freeNeighbourBitMaps.top()[bitMapIdx]|=(1<<bitPos);
    }
}

// ---- bit manipulations ----

unsigned Omega2::popCnt64(uint64_t i) {
    // SWAR algorithm
    i = i - ((i >> 1) & 0x5555555555555555);
    i = (i & 0x3333333333333333) + ((i >> 2) & 0x3333333333333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F0F0F0F0F) *
            0x0101010101010101) >> 56;
}

// ---- inline internal functions ----

inline Cell& Omega2::idxToCell(unsigned idx){
    return *cellVec[idx];
}

inline Cell& Omega2::axToCell(Ax ax){
    return cells[ax.q+boardSize-1][ax.q >= 0? ax.r+boardSize-1: ax.r+boardSize-1+ax.q];
}

inline unsigned Omega2::axToIdx(Ax ax) const{
    return cells[ax.q+boardSize-1][ax.q >= 0? ax.r+boardSize-1: ax.r+boardSize-1+ax.q].idx;
}

// ---- queries ----
vector<array<array<vector<double>, 2>, 2>> Omega2::getInitialPolicy(){
    // compute initial policy by simulating n random playouts and averaging the results based on the outcome
    // we could save the results to a file
    unsigned n = 10000;
    array<array<vector<double>, 2>, 2> scores;
    scores.fill({{vector<double>(cellNum, 0.5), vector<double>(cellNum, 0.5)}});

    array<vector<double>, 2> counts;
    counts.fill(vector<double>(cellNum, 1.0));

    vector<unsigned> cellIdxs;
    cellIdxs.reserve(cellNum);
    for(int idx = 0; idx < cellNum; ++idx)
        cellIdxs.push_back(idx);
    unsigned startNumSteps = numSteps;
    for(unsigned i = 0; i < n; ++i){
        random_shuffle(cellIdxs.begin(), cellIdxs.end());
        while(numSteps > 0)
            update(cellIdxs[numSteps-1] + (numSteps % 2) * cellNum);
        double outcome = getScore();
        while(numSteps < startNumSteps){
            auto move = getLastMove();
            scores[Player::WHITE][move.piece][move.idx] =
            (scores[Player::WHITE][move.piece][move.idx] * counts[move.piece][move.idx] + outcome) / (counts[move.piece][move.idx]+1);
            scores[Player::BLACK][move.piece][move.idx] =
            (scores[Player::BLACK][move.piece][move.idx] * counts[move.piece][move.idx] + 1 - outcome) / (counts[move.piece][move.idx]+1);
            ++counts[move.piece][move.idx];
            undo();
        }
    }
    playerScores = {0, 0};
    return vector<array<array<vector<double>, 2>, 2>>(cellNum, scores);
}

Omega2::Player Omega2::getCurrentPlayer() const{
    return currentPlayer;
}

Omega2::Player Omega2::getPreviousPlayer() const{
    return previousPlayer;
}

Omega2::Player Omega2::getLeader() const{
    if(playerScores[Player::WHITE] > playerScores[Player::BLACK])
        return Player::WHITE;
    else if(playerScores[Player::WHITE] < playerScores[Player::BLACK])
        return Player::BLACK;
    return Player::DRAW;
}

Omega2::Piece Omega2::getCurrentPiece() const{
    return currentPiece;
}

double Omega2::getScore() const{
    if(playerScores[Player::WHITE] > playerScores[Player::BLACK])
        return 1.0;
    else if(playerScores[Player::WHITE] < playerScores[Player::BLACK])
        return 0.0;
    return 0.5;
}

array<double, 2> Omega2::getPlayerScores() const{
    return playerScores;
}

Omega2::Move Omega2::getLastMove() const{
    unsigned idx = moveIdxs.back();
    return Move(previousPlayer, currentPiece == WHITEPIECE?BLACKPIECE:WHITEPIECE, idx%cellNum);
}

unsigned Omega2::getLastPieceMoveIdx() const{
    return moveIdxs.back()%cellNum;
}

unsigned Omega2::getLastMoveIdx() const{
    return moveIdxs.back();
}

Omega2::ValidMoves Omega2::getValidMoveIdxs() const{
    return validMoves;
}

vector<Omega2::Move> Omega2::getValidMoves() const{
    vector<Omega2::Move> res;
    res.reserve(validMoves.mSize);
    unsigned corr = currentPiece == WHITEPIECE?0:cellNum;
    for(const unsigned idx : validMoves){
        res.emplace_back(currentPlayer, currentPiece, idx - corr);
    }
    return res;
}

vector<Omega2::Move> Omega2::getTakenMoves() const{
    vector<Omega2::Move> res;
    res.reserve(cellNum - validMoves.mSize);
    Piece piece = Piece::WHITEPIECE;
    Player player = Player::WHITE;
    for(unsigned i= 0; i<cellNum - validMoves.mSize;++i){
        res.emplace_back(player, piece, validMoves.takenCells[i]);
        piece = (Piece)(1 - piece);
        if(piece == Player::WHITE)
            player = (Player)(1 - player);
    }
    return res;
}

unsigned Omega2::getPieceMoveIdx(unsigned moveIdx) const{
    return moveIdx % cellNum;
}

unsigned Omega2::getMoveIdx(Piece piece, unsigned pieceMoveIdx) const{
    // for Omega2 color alone identifies piece type
    return pieceMoveIdx + piece * cellNum;
}

unsigned Omega2::getRandomMove() const {
    return validMoves.getRandomMove();
}

unsigned Omega2::getNumExpectedMoves() const{
    return (numSteps + 2) / 4;
}

unsigned Omega2::getMaxNumMoves() const{
    return (numSteps + 2) / 4;
}

unsigned Omega2::getMoveNum() const{
    return cellNum*2;
}

unsigned Omega2::getMaxLegalMoveNum() const{
    return cellNum;
}

vector<Omega2::Piece> Omega2::getAvailablePieces() const{
    return {currentPiece};
}

unsigned Omega2::getCurrentDepth() const{
    return depth;
}

bool Omega2::end() const{
    return numSteps == 0;
}

// ---- internal functions ----

Omega2::ValidMoves::Cell::Cell(unsigned idx):
    idx{idx},
    prev{nullptr},
    next{nullptr}
{}

Omega2::ValidMoves::ValidMoves(unsigned cellNum):
    cellNum{cellNum},
    color{0}
{
    mSize = cellNum;
    takenCells.resize(mSize);
    cells.reserve(cellNum);
    lookup.reserve(cellNum);
    for(unsigned i=0; i<cellNum; ++i)
        cells.push_back({i});
    // produce random order
    // random_shuffle(cells.begin(), cells.end());
    for(unsigned idx=0; idx<cellNum; ++idx){
        for(unsigned i=0; i<cellNum; ++i){
            if(cells[i].idx == idx)
                lookup.push_back(&cells[i]);
        }
    }
    // set up the chain of pointers
    first = &cells[0];
    cells[0].next = &cells[1];
    for(unsigned i=1; i<cellNum-1; ++i){
        cells[i].prev = &cells[i-1];
        cells[i].next = &cells[i+1];
    }
    cells[cellNum-1].prev = &cells[cellNum-2];
}

void Omega2::ValidMoves::remove(unsigned idx){
    color = color == 1 ? 0 : 1;
    // if it is not the first item we set the previous item
    if(lookup[idx]->prev)
        lookup[idx]->prev->next = lookup[idx]->next;
    else{
        first = first->next;
    }
    // if it is not the last item we set the next item
    if(lookup[idx]->next)
        lookup[idx]->next->prev = lookup[idx]->prev;
    takenCells[cellNum - mSize] = idx;
    --mSize;
}

unsigned Omega2::ValidMoves::getRandomMove() const {
    return first->idx + color * cellNum;
}

void Omega2::ValidMoves::undo(){
    color = color == 1 ? 0 : 1;
    ++mSize;
    unsigned prev = takenCells[cellNum - mSize];
    // if there is no free cells
    if(!first){
        first = lookup[prev];
        return;
    }
    // if it was the first
    if(!lookup[prev]->prev){
        first->prev = lookup[prev];
        first = lookup[prev];
    } // if it was the last
    else if(!lookup[prev]->next){
        lookup[prev]->prev->next = lookup[prev];
    } // it was an intermediate item
    else{
        lookup[prev]->prev->next = lookup[prev];
        lookup[prev]->next->prev = lookup[prev];
    }
}

unsigned Omega2::ValidMoves::size() const{
    return mSize;
}

unsigned Omega2::ValidMoves::prevCellIdx() const{
    return takenCells[cellNum - mSize];
}

// ---- Cell and Group structs ----

Cell::Cell(int q, int r, unsigned idx):
    q{q},
    r{r},
    idx{idx},
    color{Omega2::Player::DRAW},
    groupId{-1}
{}

Group::Group(const unsigned id, const unsigned newGroupSize):
    id{id},
    size{newGroupSize}
{}

unsigned Cell::findSuperGroup(unsigned id, const vector<Group>& groups) const
{
    while(true)
    {
        // trace back the group id of the connected cells
        unsigned superId = groups[id].id;
        if(id == superId) return superId;
        id = superId;
    }
}

list<unsigned> Cell::getNeighbourGroupIds(const vector<Group>& groups, Omega2::Player color) const
{
    list<unsigned> neighbourSuperGroups;
    auto it = neighbours.begin();
    unsigned nGroupId;
    unsigned firstNGroupId = -1;
    unsigned counter = 0;
    // we use a counter instead of comparing with the iterator of end() because step size can be 2
    while(counter < neighbours.size()){
        if((*it)->color == color){
            nGroupId = findSuperGroup((*it)->groupId, groups);
            // first valid group that is connected
            if(firstNGroupId == -1){
                firstNGroupId = nGroupId;
                neighbourSuperGroups.push_back(firstNGroupId);
            }
            // second group should have different id
            else if(firstNGroupId != nGroupId){
                neighbourSuperGroups.push_back(nGroupId);
                counter += 2;
                std::advance(it, 2); // skip the next one as they would have the same group id
                break;
            }
            ++counter;
            ++it; // skip the next one as they would have the same group id
        }
        ++counter;
        ++it;
    }
    unsigned thirdNGroupId;
    // the third one should be different from the previous 2 groups
    while(counter < neighbours.size()){
        if((*it)->color == color){
            thirdNGroupId = findSuperGroup((*it)->groupId, groups);
            if(thirdNGroupId != nGroupId and thirdNGroupId != firstNGroupId){
                neighbourSuperGroups.push_back(thirdNGroupId);
                return neighbourSuperGroups; // there are maximum 3 distinct groups
            }
        }
        ++counter;
        ++it;
    }

    return neighbourSuperGroups;
}
