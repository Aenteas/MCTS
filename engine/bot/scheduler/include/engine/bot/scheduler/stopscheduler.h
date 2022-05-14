#ifndef STOPSCHEDULER_H
#define STOPSCHEDULER_H

#include <math.h>
#include <chrono>

// type_trait to get node type
template<typename T>
struct extractNodeType //lets call it extract_value_type
{
    typedef T value_type;
};

template<template<typename> class T, typename N>
struct extractNodeType<T<N>>   //specialization
{
    typedef N value_type;
};

template<typename G, typename T>
class StopScheduler{
public:
    StopScheduler(const std::chrono::milliseconds& timeLeft,
                  G& game,
                  T& table,
                  double p=0.9,
                  unsigned freq=100,
                  int reserveTime=2000);

    StopScheduler(const StopScheduler&)=delete;
    StopScheduler& operator=(const StopScheduler&)=delete;
    StopScheduler(StopScheduler&&)=delete;
    StopScheduler& operator=(StopScheduler&&)=delete;

    virtual ~StopScheduler()=default;

    bool finish();
    void schedule();

protected:
    // p * msecsBudget time is given to the second best child to catch up
    const double p;
    // frequency of checking stop condition. Make sure that reserveTime is large enough to avoid running out of time
    const unsigned freq;
    int reserveTime;
    // round per milliseconds
    double speed;
    // elapsed time in milliseconds from the start of the current round
    unsigned elapsedmsecs;
    // base time devoted for the current round
    unsigned msecsBudget;
    // number of playouts since the beginnign of the current round
    double numPlayouts;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    double n;

    // parabolic parameters
    double a;
    double b;
    double c;
    double w;
    const std::chrono::milliseconds& timeLeft;
    G& game;
    T& table;
};

template<typename G, typename T>
StopScheduler<G, T>::StopScheduler(const std::chrono::milliseconds& timeLeft,
                                G& game,
                                T& table,
                                double p,
                                unsigned freq,
                                int reserveTime):
    timeLeft(timeLeft),
    game(game),
    table(table),
    p(p),
    freq(freq),
    reserveTime(reserveTime)
{
    if(!(p >= 0 or p<=1))
        throw std::invalid_argument( "p argument should be greater than 0 and smaller or equal to 1" );
    if(reserveTime <= 0)
        throw std::invalid_argument( "reserveTime argument should be greater than 0" );
    if(freq < 2)
        throw std::invalid_argument( "freq argument should be at least 2" );
    // compute parabolic time distrubution: m is for the middle and s is for the starting move
    // we are fitting a parabolic curve to 3 points: (x1,y1), (x2,y2), (x3,y3)
    n = game.getNumExpectedMoves();
    double m = 1.0 + (n/2.0-1.0)/2;
    double s = 1.0;
    double x1 = 1.0;
    double y1 = 1.0;
    double x2 = (1.0+n)/2.0;
    double y2 = m;
    double x3 = n;
    double y3 = s;

    double denom = (x1 - x2)*(x1 - x3)*(x2 - x3);
    a = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
    b = (x3*x3 * (y1 - y2) + x2*x2 * (y3 - y1) + x1*x1 * (y2 - y3)) / denom;
    c = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denom;

    // check if slope of parabola is under the y=x line so we can not run out of time
    if(2*a+b >= 1)
        throw std::invalid_argument( "invalid parabolic curve, lower the value of m" );
    // m is assumed to be greater than s: we use time budget
    if(s >= m)
        throw std::invalid_argument( "invalid parabolic curve, s should be smaller than m" );
    // m and s should be greater than 0
    if(s <= 0.0 || m <= 0.0)
        throw std::invalid_argument( "invalid parabolic curve, s and m should be greater than 0" );
}

template<typename G, typename T>
bool StopScheduler<G, T>::finish(){
    ++numPlayouts;
    // Make sure that reserve time is large enough to run full cycles at least frequency times otherwise
    // it is not quaranteed that the AI does not run out of time
    if(fmod(numPlayouts+1, freq) != 0.0)
        return false;
    auto now = std::chrono::steady_clock::now();
    elapsedmsecs = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    // if the time spent for current search is more than the budget
    if(msecsBudget <= elapsedmsecs){
        return true;
    }
    speed = numPlayouts / elapsedmsecs;
    double maxScore = -1;
    double secondMaxScore = -1;
    double score;

    typename extractNodeType<T>::value_type* bestNode;
    typename extractNodeType<T>::value_type* secondBestNode;
    for(unsigned moveIdx : game.getValidMoveIdxs()){
        auto node = table.select(moveIdx);
        score = node ? node->getVisitCount() : 0;
        if(score > maxScore){
            secondMaxScore = maxScore;
            maxScore = score;
            secondBestNode = bestNode;
            bestNode = node;
        }
        else if(score > secondMaxScore){
            secondMaxScore = score;
            secondBestNode = node;
        }
    }
    if(bestNode){
        // most likely there is no way for the AI to win
        if(bestNode->getStateScore() < 0.01 and elapsedmsecs >= 500){
            return true;
        }
        // most likely the AI won
        if(bestNode->getStateScore() > 0.99 and elapsedmsecs >= 500){
            return true;
        }
    }
    // check if the best node can change within the dedicated time frame
    // estimate  of minimum number of playouts to change the best node (with regard to the visit count)
    double minPlayouts = maxScore - secondMaxScore;
    // check if the expected number of playouts that can be carried out within the dedicated time frame is smaller
    if(minPlayouts > p / w * speed * (msecsBudget - elapsedmsecs)){
        return true;
    }
    return false;
}

template<typename G, typename T>
void StopScheduler<G, T>::schedule(){
    numPlayouts = -1;
    startTime = std::chrono::steady_clock::now();
    int rmsecs = timeLeft.count() - reserveTime;
    n = game.getNumExpectedMoves();
    w  = (a*n*n + b*n + c);
    msecsBudget = w / n * (rmsecs > 0 ? rmsecs : 1);
}

#endif // STOPSCHEDULER_H
