#ifndef BOARDDIALOG_H
#define BOARDDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QTimer>
#include <QTime>
#include <QObject>
#include <QThread>

#include <boost/optional.hpp>
#include <memory>
#include <stdexcept>

#include "canvas.h"
#include "engine/bot/mcts/base/mctsbot.h"
#include "engine/bot/base/aibotbase.h"
#include "engine/bot/random/randombot.h"

#include "engine/game/omega/omega.h"

struct ComputerData{
    QString version, node, policy;
    bool recycling;
    unsigned budget;
};

struct Player;
struct Human;
struct Computer;

namespace Ui {
class BoardDialog;
}

class BoardDialog : public QDialog
{
    Q_OBJECT
    friend class AiBot;
    friend class Player;
    friend class Human;
    friend class Computer;
protected:
    void setEnabled(bool flag);

public:
    BoardDialog(
        QWidget *parent,
        unsigned boardSize,
        double radius,
        double padding,
        unsigned timeWhite,
        unsigned timeBlack,
        const boost::optional<ComputerData>& paramsWhite,
        const boost::optional<ComputerData>& paramsBlack
    );
    ~BoardDialog()=default;

    BoardDialog(const BoardDialog&)=delete;
    BoardDialog& operator=(const BoardDialog&)=delete;

private slots:
    void on_quitButton_clicked();

    void on_startButton_clicked();

    void updateCountdown();

public slots:
    void updateByHuman(unsigned piece, unsigned cellIdx);
    void updateByComputer();

private:
    // ---- control panel ----
    void updateControlPanel();
    void freezeControlPanel(const unsigned winner);

    // ---- ui ----
    std::shared_ptr<Ui::BoardDialog> ui;
    std::shared_ptr<Canvas> canvas;

    // ---- config ----
    unsigned boardSize;

    // ---- game state ----
    unsigned currPlayer;
    bool inGame;
    std::shared_ptr<Omega> game;

    std::array<std::shared_ptr<Player>, 2> players;

signals:
    void back_to_main();
};

struct Player: public QObject{
    BoardDialog& board;
    unsigned startingSecTime;
    QTimer timer;
    QTime remainingTime;

    static std::shared_ptr<Player> create(
        BoardDialog& board, 
        unsigned startingSecTime, 
        const boost::optional<ComputerData>& params);

    virtual void update(unsigned moveIdx)=0;
    virtual void play()=0;
    virtual void stop()=0;
    virtual void reset()=0;
    virtual boost::optional<std::string> getErrorMsg() const=0;
    virtual bool isInterrupted() const{
        return false;
    }
protected:
    void initTimer(){
        timer.stop();
        remainingTime = QTime(0, startingSecTime / 60, startingSecTime % 60);
    }
    void startTiming(){
        auto t = timer.remainingTime() <= 0 ? 1000 : timer.remainingTime();
        timer.start(t);
    }
    Player(BoardDialog& board, unsigned startingSecTime):
        QObject(nullptr),
        board(board), 
        startingSecTime(startingSecTime),
        timer(QTimer(&board)){
        initTimer();
        board.connect(&timer, SIGNAL(timeout()), &board, SLOT(updateCountdown()));
    }
};

struct Human : public Player{
    Human(BoardDialog& board, unsigned startingSecTime):
        Player(board, startingSecTime) {}

    virtual void update(unsigned moveIdx) override { // empty 
    }
    virtual void play() override{
        board.setEnabled(true);
        startTiming();
    }
    virtual void stop() override{
        timer.stop();
        board.canvas->active = false;
    }
    virtual void reset() override{
        initTimer();
    }
    virtual boost::optional<std::string> getErrorMsg() const override{
        return boost::none;
    }
};

struct Computer : public Player{
    Q_OBJECT
protected:
    friend class BoardDialog;
    ComputerData params;
    std::shared_ptr<AiBotBase> impl;
    boost::optional<std::string> errorMsg;
    QThread thread;
    bool interrupted;

    std::chrono::milliseconds computeTimeLeft() const{
        int millisecs = 3.6e6 * remainingTime.hour() + 6e4 * remainingTime.minute() + 1e3 * remainingTime.second() + remainingTime.msec();
        return std::chrono::milliseconds(millisecs);
    }

signals:
    void finished();

private slots:
    void updateGameSlot(){
        try{
            impl->setTimeLeft(computeTimeLeft());
            impl->updateGame();
        }
        catch(const std::exception& e){
            errorMsg = e.what();
        }
        catch(...){
            errorMsg = "Unknown error during execution.";
        }

        emit finished();
    }

public:
    Computer(BoardDialog& board, unsigned startingSecTime, const ComputerData& params):
        Player(board, startingSecTime),
        params(params),
        impl(nullptr),
        interrupted(false)
    {
        reset();
        this->moveToThread(&thread);
        connect(&thread, SIGNAL (started()), this, SLOT (updateGameSlot()));
        connect(this, SIGNAL (finished()), &thread, SLOT (quit()));
    }

    virtual void update(unsigned moveIdx) override{
        impl->updateByOpponent(moveIdx);
    }
    virtual void play() override{
        board.setEnabled(false);
        startTiming();
        interrupted = false;
        thread.start();
    }
    virtual void stop() override{
        interrupted = true;
        impl->stop();
        timer.stop();
    }
    virtual void reset() override{
        impl = [this]() -> std::shared_ptr<AiBotBase>{
            if(params.version == "MCTS")
                return std::make_shared<MCTSBot>(*(board.game), params.node.toStdString(), params.policy.toStdString(), params.recycling, params.budget);
            else if(params.version == "random")
                return std::make_shared<RandomBot<Omega>>(*(board.game));
            else
                throw std::invalid_argument( "Invalid input string: " + params.version.toStdString() + " received" );
        }();
        board.connect(&(thread), SIGNAL (finished()), &board, SLOT(updateByComputer()));
        initTimer();
    }
    virtual boost::optional<std::string> getErrorMsg() const override{
        return errorMsg;
    }
    virtual bool isInterrupted() const override{
        return interrupted;
    }
};

#endif // BOARDDIALOG_H
