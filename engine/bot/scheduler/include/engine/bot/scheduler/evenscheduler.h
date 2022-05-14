#ifndef EVENSCHEDULER_H
#define EVENSCHEDULER_H

#include <chrono>
#include <math.h>
#include <stdexcept>

template<typename G>
class EvenScheduler
{
public:
    EvenScheduler(const std::chrono::milliseconds& timeLeft, G* game, unsigned int freq=100 , int reserveTime=2000);
    virtual ~EvenScheduler()=default;

    bool finish();
    void schedule();
protected:
    // frequency of checking stop condition. Make sure that reserveTime is large enough to avoid running out of time
    unsigned int freq;
    int reserveTime;
    // elapsed time in milliseconds from the start of the current round
    unsigned int elapsedmsecs;
    // base time devoted for the current round
    unsigned int msecsBudget;
    // number of playouts since the beginnign of the current round
    double numPlayouts;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    G* game;
    const std::chrono::milliseconds& timeLeft;
};

template<typename G>
EvenScheduler<G>::EvenScheduler(const std::chrono::milliseconds& timeLeft, G* game, unsigned int freq , int rTime):
    timeLeft{timeLeft},
    game{game},
    freq{freq},
    reserveTime{rTime}
{
    if(eserveTime <= 0)
        throw std::invalid_argument( "reserveTime argument should be greater than 0" );
    if(freq < 2)
        throw std::invalid_argument( "freq argument should be at least 2" );
}

template<typename G>
bool EvenScheduler<G>::finish(){
    ++numPlayouts;
    if(fmod(numPlayouts+1, freq) != 0.0)
        return false;
    auto now = std::chrono::steady_clock::now();
    elapsedmsecs = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();

    // if the time spent for current search is more than the budget
    if(msecsBudget <= elapsedmsecs)
        return true;
    return false;
}

template<typename G>
void EvenScheduler<G>::schedule(){
    numPlayouts = -1;
    startTime = std::chrono::steady_clock::now();
    int rmsecs = timeLeft.count() - reserveTime;
    double numMoves = game->numExpectedMoves();
    msecsBudget = rmsecs / numMoves;
}

#endif // EVENSCHEDULER_H
