#ifndef BOARDDIALOG_H
#define BOARDDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QTimer>
#include <QTime>

#include <memory>
#include <stdexcept>

#include "canvas.h"
#include "engine/bot/mcts/base/mctsbot.h"
#include "engine/bot/base/aibotbase.h"
#include "qtbotwrapper.h"
#include "engine/bot/random/randombot.h"

#include "engine/game/omega/omega.h"

namespace Ui {
class BoardDialog;
}

class BoardDialog : public QDialog
{
    Q_OBJECT
    friend class AiBot;
public:
    struct ComputerData{
        QString version, node, policy;
        bool recycling;
        unsigned budget;
    };
protected:
    void setEnabled(bool flag);
    struct Computer;
    struct Human;

    struct Player{
        BoardDialog& board;
        unsigned startingSecTime;
        QTimer timer;
        QTime remainingTime;

        static std::shared_ptr<Player> create(
            BoardDialog& board, 
            unsigned startingSecTime, 
            const boost::optional<ComputerData>& params)
        {
            if(params)
                return std::make_shared<Computer>(board, startingSecTime, *params);
            else
                return std::make_shared<Human>(board, startingSecTime);
        }

        virtual void update(unsigned moveIdx)=0;
        virtual void play()=0;
        virtual void stop()=0;
        virtual void reset()=0;
        virtual boost::optional<std::string> getErrorMsg() const=0;
    protected:
        void initTimer(){
            timer.stop();
            remainingTime = QTime(0, startingSecTime / 60, startingSecTime % 60);
        }
        void startTimer(){
            auto t = timer.remainingTime() <= 0 ? 1000 : timer.remainingTime();
            timer.start(t);
        }
        Player(BoardDialog& board, unsigned startingSecTime): 
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
            startTimer();
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
        ComputerData params;
        std::shared_ptr<QtBotWrapper> impl;

        Computer(BoardDialog& board, unsigned startingSecTime, const ComputerData& params):
            Player(board, startingSecTime),
            params(params),
            impl(nullptr)
        {
            reset();
        }

        virtual void update(unsigned moveIdx) override{
            impl->updateByOpponent(moveIdx);
        }
        virtual void play() override{
            board.setEnabled(false);
            startTimer();
            impl->play();
        }
        virtual void stop() override{
            impl->stop();
            timer.stop();
        }
        virtual void reset() override{
            auto aiBotImpl = [this]() -> AiBotBase*{
                if(params.version == "MCTS")
                    return new MCTSBot(*(board.game), params.node.toStdString(), params.policy.toStdString(), params.recycling, params.budget);
                else if(params.version == "random")
                    return new RandomBot<Omega>(*(board.game));
                else
                    throw std::invalid_argument( "Invalid input string: " + params.version.toStdString() + " received" );
            }();
            impl = std::make_shared<QtBotWrapper>(aiBotImpl, remainingTime);
            board.connect(&(impl->thread), SIGNAL (finished()), &board, SLOT(updateByComputer()));
            initTimer();
        }
        virtual boost::optional<std::string> getErrorMsg() const override{
            return impl->getErrorMsg();
        }
    };

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

#endif // BOARDDIALOG_H
