#ifndef MCTSBOT_H
#define MCTSBOT_H

#include <chrono>
#include <unistd.h>

// Type erasure
class MCTSBase{
public:
    MCTSBase()=default;
    ~MCTSBase() { delete impl; }

    MCTSBase(const MCTSBase&)=delete;
    MCTSBase& operator=(const MCTSBase&)=delete;
    MCTSBase(const MCTSBase&&)=delete;
    MCTSBase& operator=(const MCTSBase&&)=delete;

    virtual void run()=0;
    virtual void updateRoot(unsigned moveIdx)=0;
};

class MCTSBot: public AiBotBase
{
public:
    MCTSBot(std::string game, std::string node, bool recycling, unsigned budget);
    virtual ~MCTSBot()=default;

    MCTSBot(const MCTSBot&)=delete;
    MCTSBot& operator=(const MCTSBot&)=delete;
    MCTSBot(const MCTSBot&&)=delete;
    MCTSBot& operator=(const MCTSBot&&)=delete;

    virtual void update(unsigned moveIdx) override;
    void updateGame() override;

private:
    MCTSBase* impl;
};

#endif // MCTSBOT_H
