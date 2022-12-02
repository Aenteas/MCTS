#include "boarddialog.h"
#include "ui_boarddialog.h"

using namespace std;

BoardDialog::BoardDialog(
    QWidget *parent,
    unsigned boardSize,
    double radius,
    double padding,
    QString mode,
    QString color,
    unsigned time,
    QString bot,
    QString node,
    bool recycling,
    unsigned budget
    ) :
    QDialog(parent),
    ui(new Ui::BoardDialog),
    boardSize(boardSize),
    bot(bot),
    node(node),
    recycling(recycling),
    budget(budget),
    canvas(new Canvas(this, boardSize, radius, padding)),
    time{time},
    inGame{false},
    game(new Omega(boardSize)),
    rTimeWhite{0},
    rTimeBlack{0},
    whiteTimer{nullptr},
    blackTimer{nullptr}
{
    // setup UI and connections
    playerColor = color == "White" ? 0 : 1;
    connect(this, SIGNAL (back_to_main()), parent, SLOT (back_to_main()));
    if(mode == "vs AI"){
        initBot();
    }
    else aiBot = nullptr;
    ui->setupUi(this);
    this->setWindowTitle(QString("Omega"));
    ui->horizontalLayout->addWidget(canvas);
    setMinimumWidth(canvas->board_width()+200);
    setMinimumHeight(canvas->board_height()+50);
    setMaximumWidth(canvas->board_width()+160);
    setMaximumHeight(canvas->board_height()+50);
    ui->controlPanel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

BoardDialog::~BoardDialog()
{
    if(aiBot)
        delete aiBot;
    delete canvas;
    delete ui;
    delete game;
}

void BoardDialog::initBot(){
    auto aiBotImpl = [this]() -> AiBotBase*{
        if(bot == "MCTS")
            return new MCTSBot(*game, node.toStdString(), recycling, budget);
        else if(bot == "Random")
            return new RandomBot<Omega>(*game);
    }();
    aiBot = new QtBotWrapper(aiBotImpl, playerColor == 0 ? timeBlack : timeWhite);
    connect(&(aiBot->thread), SIGNAL (finished()), this, SLOT(updateFromAiBot()));
}

void BoardDialog::initTimers(){
    // init time in minutes
    timeWhite = QTime(0, time, 0);
    timeBlack = QTime(0, time, 0);

    rTimeBlack = 0;
    rTimeBlack = 0;
    whiteCounter = 1000;
    blackCounter = 1000;
    if(whiteTimer) delete whiteTimer;
    if(blackTimer) delete blackTimer;
    whiteTimer = new QTimer(this);
    blackTimer = new QTimer(this);

    ui->whiteCountDown->setText(timeWhite.toString("m:ss"));
    ui->blackCountDown->setText(timeBlack.toString("m:ss"));
    connect(whiteTimer, SIGNAL(timeout()), this, SLOT(updateCountdown()));
    connect(blackTimer, SIGNAL(timeout()), this, SLOT(updateCountdown()));
}

void BoardDialog::startTimer(bool restart){
    if(restart) initTimers();
    if(game->getNextPlayer() == 0){
        if(restart or rTimeWhite == 0)
            whiteTimer->start(1);
        else
            whiteTimer->start(rTimeWhite);
    }
    else{
        if(restart or rTimeBlack == 0)
            blackTimer->start(1);
        else
            blackTimer->start(rTimeBlack);
    }
}

void BoardDialog::stopTimer(){
    if(game->getNextPlayer() == 0){
        if(whiteTimer){
            rTimeWhite = whiteTimer->remainingTime();
            whiteTimer->stop();
        }
    }
    else if(blackTimer){
        rTimeBlack = blackTimer->remainingTime();
        blackTimer->stop();
    }
}

void BoardDialog::switchTimers(){
    // stop timer of previous player
    if(game->getNextPlayer() == 1){
        rTimeWhite = whiteTimer->remainingTime();
        whiteTimer->stop();
    }
    else{
        rTimeBlack = blackTimer->remainingTime();
        blackTimer->stop();
    }
    startTimer(false);
}

void BoardDialog::on_startButton_clicked()
{
    canvas->active = false;
    stopTimer();
    if(inGame){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Restart", "Are you sure want to restart the game?", QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            delete game;
            game = new Omega(boardSize);
            updateControlPanel();
            canvas->reset();
            startTimer(true);
            // start bot if there is one
            if(aiBot){
                delete aiBot;
                initBot();
                // if bot starts the game
                if(playerColor != 0){
                    ui->quitButton->setEnabled(false);
                    ui->startButton->setEnabled(false);
                    canvas->active = false;
                    aiBot->updateGame();
                }
                else canvas->active = true;
            }
            else canvas->active = true;
        }
        else if(!game->end()){
            startTimer(false);
            canvas->active = true;
        }
    }
    else{
        inGame = true;
        ui->startButton->setText("Start New Game");
        if(aiBot){
            ui->description->setText("Setup ...");
            ui->description->repaint();
        }
        updateControlPanel();
        startTimer(true);
        // start bot if there is one
        if(aiBot){
            // if bot starts the game
            if(playerColor != 0){
                ui->quitButton->setEnabled(false);
                ui->startButton->setEnabled(false);
                canvas->active = false;
                aiBot->updateGame();
            }
            else canvas->active = true;
        }
        else canvas->active = true;
    }
}

void BoardDialog::on_quitButton_clicked()
{
    stopTimer();
    canvas->active = false;
    if(inGame){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Quit", "Are you sure want to quit?", QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes)
            emit back_to_main();
        else if(!game->end()){
            startTimer(false);
            canvas->active = true;
        }
    }
    else emit back_to_main();
}

void BoardDialog::updateControlPanel(){
    auto scores = game->scores();
    if(game->getCurrentDepth() == 0){
        ui->whiteScore->display(0);
        ui->blackScore->display(0);
    }
    else if(game->getCurrentDepth() == 1){
        ui->whiteScore->display(1);
        ui->blackScore->display(0);
    }
    else{
        ui->whiteScore->display(scores[0]);
        ui->blackScore->display(scores[1]);
    }
    ui->whiteScore->repaint();
    ui->blackScore->repaint();
    unsigned leader = 2; // draw
    if(scores[0] > scores[1])
        leader = 0;
    else if(scores[0] < scores[1])
        leader = 1;
    if(game->end()){
        freezeControlPanel(leader);
    }
    else{
        if(aiBot){
            QString description = game->getNextPlayer() == playerColor ? QString("your") : QString("the AI's");
            ui->description->setText("It's " + description + " turn!");
        }
        else{
            if(game->getNextPlayer() == 0){
                ui->description->setText("It's WHITE's turn!");
            }
            else{
                 ui->description->setText("It's BLACK's turn!");
            }
        }
        ui->description->repaint();
    }
}

void BoardDialog::freezeControlPanel(const unsigned winner){
    QString description;
    if(aiBot){
        if(winner == playerColor)
            description = "You won";
        else if(winner == 2)
            description = "Draw!";
        else
            description = "The AI won";
    }
    else{
        if(winner == 0)
            description = "White won";
        else if(winner == 1)
            description = "Black won";
        else
            description = "Draw!";
    }
    ui->description->setText(description);
    canvas->active = false;
    whiteTimer->stop();
    blackTimer->stop();
    ui->description->repaint();
    ui->quitButton->setEnabled(true);
    ui->startButton->setEnabled(true);
}

void BoardDialog::updateCountdown()
{
    /*
     * Timers are updated in every 1 millisec but only the seconds are shown on the UI
     * This way the bot has sub-sec resolution and can better schedule the time for each step
    */
    if(whiteTimer->isActive()){
        timeWhite = timeWhite.addMSecs(-1);
        --whiteCounter;
        if(whiteCounter == 0){
            ui->whiteCountDown->setText(timeWhite.toString("m:ss"));
            whiteCounter = 1000;
        }
        if(timeWhite.toString("m:ss:z") == "0:00:0"){
            freezeControlPanel(1);
        }
        else whiteTimer->start(1);
    }
    else{
        timeBlack = timeBlack.addMSecs(-1);
        --blackCounter;
        if(blackCounter == 0){
            ui->blackCountDown->setText(timeBlack.toString("m:ss"));
            blackCounter = 1000;
        }
        if(timeBlack.toString("m:ss:z") == "0:00:0"){
            freezeControlPanel(0);
        }
        else blackTimer->start(1);
    }
}

void BoardDialog::updateGameState(unsigned piece, unsigned cellIdx){
    unsigned moveIdx = game->toMoveIdx(piece, cellIdx);
    game->update(moveIdx);
    // handles the case when game ends
    updateControlPanel();
    if(!game->end()){
        if(piece == 1)
            switchTimers();
        if(aiBot){
            aiBot->updateByOpponent(moveIdx);
            // aibot's turn
            if(piece == 1){
                canvas->active = false;
                ui->quitButton->setEnabled(false);
                ui->startButton->setEnabled(false);
                // store player as we simultaneously update gamestate from the AI
                aiBot->updateGame();
            }
        }
    }
}

void BoardDialog::updateFromAiBot(){
    boost::optional<std::string> msgOpt = aiBot->getErrorMsg();
    if(msgOpt){
        QMessageBox msgBox;
        msgBox.setText(QString::fromStdString(*msgOpt));
        msgBox.exec();
        emit back_to_main();
        return;
    }
    const auto& moves = game->getTakenMoves().crbegin();
    // get last 2 moves to update
    unsigned blackCell = moves.getPos();
    --moves;
    canvas->aiBotMovedEvent(moves.getPos(), blackCell);
    switchTimers();
    // handles the case when game ends
    updateControlPanel();
    ui->quitButton->setEnabled(true);
    ui->startButton->setEnabled(true);
    if(!game->end()) canvas->active = true;
}
