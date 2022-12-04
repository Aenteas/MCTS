#ifndef BOARDDIALOG_H
#define BOARDDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QTimer>
#include <QTime>

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
    explicit BoardDialog(QWidget *parent,
    unsigned boardSize,
    double radius,
    double padding,
    QString mode,
    QString color,
    unsigned time,
    QString bot,
    QString node,
    QString policy,
    bool recycling,
    unsigned budget);
    ~BoardDialog();

    BoardDialog(const BoardDialog&)=delete;
    BoardDialog& operator=(const BoardDialog&)=delete;

private slots:
    void on_quitButton_clicked();

    void on_startButton_clicked();

    void updateCountdown();

public slots:
    void updateGameState(unsigned piece, unsigned cellIdx);
    void updateFromAiBot();

private:
    // ---- timers ----
    void initTimers();
    void startTimer(bool restart);
    void stopTimer();
    void switchTimers();

    // ---- control panel ----
    void updateControlPanel();
    void freezeControlPanel(const unsigned winner);

    // ---- ui ----
    Ui::BoardDialog *ui;
    Canvas* canvas;

    // ---- config ----
    unsigned boardSize;
    QString bot;
    QString node;
    QString policy;
    bool recycling;
    unsigned budget;

    inline void initBot();

    // ---- game state ----
    unsigned playerColor;
    bool inGame;
    Omega* game;
    QtBotWrapper* aiBot;

    // ---- timers ----
    unsigned time;
    unsigned rTimeWhite;
    unsigned rTimeBlack;
    QTimer* whiteTimer;
    QTimer* blackTimer;
    unsigned whiteCounter;
    unsigned blackCounter;
    QTime timeWhite;
    QTime timeBlack;
    unsigned currPlayer;

signals:
    void back_to_main();
};

#endif // BOARDDIALOG_H
