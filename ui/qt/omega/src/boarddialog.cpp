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
    playerColor = color == "White" ? Omega::Player::WHITE : Omega::Player::BLACK;
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
    aiBot = new QtBotWrapper(aiBotImpl, playerColor == Omega::WHITE ? timeBlack : timeWhite);
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
    if(game->getCurrentPlayer() == Omega::Player::WHITE){
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
    if(game->getCurrentPlayer() == Omega::Player::WHITE){
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
    if(game->getCurrentPlayer() == Omega::Player::BLACK){
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
                if(playerColor != Omega::Player::WHITE){
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
            if(playerColor != Omega::Player::WHITE){
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
    ui->whiteScore->display(game->getPlayerScores()[Omega::Player::WHITE]);
    ui->blackScore->display(game->getPlayerScores()[Omega::Player::BLACK]);
    ui->whiteScore->repaint();
    ui->blackScore->repaint();
    if(game->end()){
        freezeControlPanel(game->getLeader());
    }
    else{
        if(aiBot){
            QString description = game->getCurrentPlayer() == playerColor ? QString("your") : QString("the AI's");
            ui->description->setText("It's " + description + " turn!");
        }
        else{
            if(game->getCurrentPlayer() == Omega::Player::WHITE){
                ui->description->setText("It's WHITE's turn!");
            }
            else{
                 ui->description->setText("It's BLACK's turn!");
            }
        }
        ui->description->repaint();
    }
}

void BoardDialog::freezeControlPanel(const Omega::Player winner){
    QString description;
    if(aiBot){
        if(winner == playerColor)
            description = "You won";
        else if(winner == Omega::Player::DRAW)
            description = "Draw!";
        else
            description = "The AI won";
    }
    else{
        if(winner == Omega::Player::WHITE)
            description = "White won";
        else if(winner == Omega::Player::BLACK)
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
            freezeControlPanel(Omega::Player::BLACK);
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
            freezeControlPanel(Omega::Player::WHITE);
        }
        else blackTimer->start(1);
    }
}

void BoardDialog::updateGameState(Omega::Piece piece, unsigned cellIdx){
    unsigned moveIdx = game->getMoveIdx(piece, cellIdx);
    game->update(moveIdx);
    // handles the case when game ends
    updateControlPanel();
    if(!game->end()){
        if(piece == Omega::BLACKPIECE)
            switchTimers();
        if(aiBot){
            aiBot->updateByOpponent(moveIdx);
            // aibot's turn
            if(piece == Omega::BLACKPIECE){
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
    auto moves = game->getTakenMoves();
    // get last 2 moves to update
    auto blackCell = moves.back().idx;
    auto whiteCell = moves[moves.size()-2].idx;
    canvas->aiBotMovedEvent(whiteCell, blackCell);
    switchTimers();
    // handles the case when game ends
    updateControlPanel();
    ui->quitButton->setEnabled(true);
    ui->startButton->setEnabled(true);
    if(!game->end()) canvas->active = true;
}
