#ifndef AIBOTBASE_H
#define AIBOTBASE_H

#include <chrono>

class AiBotBase
{
public:
    AiBotBase()=default;
    ~AiBotBase()=default;

    AiBotBase(const AiBotBase&)=delete;
    AiBotBase& operator=(const AiBotBase&)=delete;
    AiBotBase(AiBotBase&&)=delete;
    AiBotBase& operator=(AiBotBase&&)=delete;

    virtual void updateGame()=0;
    virtual void updateByOpponent(unsigned int moveIdx)=0;

    virtual void stop()=0;
    void setTimeLeft(const std::chrono::milliseconds& timeLeft);

protected:
    void startTimer();

    inline bool isTimeOut() const;

    // available time when process was started. It is updated from outside before the start of each process
    std::chrono::milliseconds timeLeft;

private:
    // exact time when process was started
    std::chrono::time_point<std::chrono::steady_clock> start;
};

#endif // AIBOTBASE_H
