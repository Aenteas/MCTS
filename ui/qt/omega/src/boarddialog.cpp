#include "boarddialog.h"
#include "ui_boarddialog.h"

using namespace std;

// ---- BoardDialog ----

BoardDialog::BoardDialog(
    QWidget *parent,
    unsigned boardSize,
    double radius,
    double padding,
    unsigned timeWhite,
    unsigned timeBlack,
    const boost::optional<ComputerData>& paramsWhite,
    const boost::optional<ComputerData>& paramsBlack
    ) :
    QDialog(parent),
    ui(make_shared<Ui::BoardDialog>()),
    boardSize(boardSize),
    canvas(make_shared<Canvas>(this, boardSize, radius, padding)),
    inGame{false},
    game(make_shared<Omega>(boardSize)),
    players{Player::create(*this, timeWhite, paramsWhite), Player::create(*this, timeBlack, paramsBlack)},
    currPlayer(0)
{
    // setup UI and connections
    connect(this, SIGNAL (back_to_main()), parent, SLOT (back_to_main()));
    ui->setupUi(this);
    this->setWindowTitle(QString("Omega"));
    ui->horizontalLayout->addWidget(addressof(*canvas));
    setMinimumWidth(canvas->board_width()+200);
    setMinimumHeight(canvas->board_height()+50);
    setMaximumWidth(canvas->board_width()+160);
    setMaximumHeight(canvas->board_height()+50);
    ui->controlPanel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

void BoardDialog::setEnabled(bool flag){
    canvas->active = flag;
}

void BoardDialog::on_startButton_clicked()
{
    players[currPlayer]->stop();
    if(inGame){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Restart", "Are you sure want to restart the game?", QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            game = nullptr; // delete shared state
            game = make_shared<Omega>(boardSize);
            players[0]->reset();
            players[1]->reset();
            ui->whiteCountDown->setText(players[0]->remainingTime.toString("m:ss"));
            ui->blackCountDown->setText(players[1]->remainingTime.toString("m:ss"));
            updateControlPanel();
            canvas->reset();
            currPlayer = 0;
            players[currPlayer]->play();
        }
        else if(!game->end())
            players[currPlayer]->play();
    }
    else{
        inGame = true;
        ui->startButton->setText("Start New Game");
        ui->whiteCountDown->setText(players[0]->remainingTime.toString("m:ss"));
        ui->blackCountDown->setText(players[1]->remainingTime.toString("m:ss"));
        updateControlPanel();
        players[currPlayer]->play();
    }
}

void BoardDialog::on_quitButton_clicked()
{
    players[currPlayer]->stop();
    if(inGame){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Quit", "Are you sure want to quit?", QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes)
            emit back_to_main();
        else if(!game->end()){
            players[currPlayer]->play();
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
        if(game->getNextPlayer() == 0){
            ui->description->setText("It's WHITE's turn!");
        }
        else{
                ui->description->setText("It's BLACK's turn!");
        }
        ui->description->repaint();
    }
}

void BoardDialog::freezeControlPanel(const unsigned winner){
    QString description;
    if(winner == 0)
        description = "White won";
    else if(winner == 1)
        description = "Black won";
    else
        description = "Draw!";
    ui->description->setText(description);
    canvas->active = false;
    players[0]->timer.stop();
    players[1]->timer.stop();
    ui->description->repaint();
    ui->quitButton->setEnabled(true);
    ui->startButton->setEnabled(true);
}

void BoardDialog::updateCountdown()
{
    players[currPlayer]->remainingTime = players[currPlayer]->remainingTime.addSecs(-1);
    auto str = players[currPlayer]->remainingTime.toString("m:ss");
    if(currPlayer == 0)
        ui->whiteCountDown->setText(str);
    else
        ui->blackCountDown->setText(str);
    if(str == "0:01")
        freezeControlPanel(1 - currPlayer);
    else 
        players[currPlayer]->timer.start(1000);
}
void BoardDialog::updateByHuman(unsigned piece, unsigned cellIdx){
    unsigned moveIdx = game->toMoveIdx(piece, cellIdx);
    game->update(moveIdx);
    players[currPlayer]->update(moveIdx);
    // handles the case when game ends
    updateControlPanel();
    if(!game->end() && piece == 1){
        players[currPlayer]->stop();
        currPlayer = 1 - currPlayer;
        players[currPlayer]->play();
    }
}
void BoardDialog::updateByComputer(){
    boost::optional<std::string> msgOpt = players[currPlayer]->getErrorMsg();
    if(msgOpt){
        QMessageBox msgBox;
        msgBox.setText(QString::fromStdString(*msgOpt));
        msgBox.exec();
        emit back_to_main();
        return;
    }
    if(!players[currPlayer]->isInterrupted()){
        auto moves = game->getTakenMoves().rbegin();
        unsigned blackPos = moves.getPos();
        --moves;
        unsigned whitePos = moves.getPos();
        // update UI
        canvas->update(whitePos, blackPos);
        updateControlPanel();
        if(!game->end()){
            players[currPlayer]->stop();
            // update opponent
            currPlayer = 1 - currPlayer;
            players[currPlayer]->update(game->toMoveIdx(0, whitePos));
            players[currPlayer]->update(game->toMoveIdx(1, blackPos));
            players[currPlayer]->play();
        }
    }
}

std::shared_ptr<Player> Player::create(
    BoardDialog& board, 
    unsigned startingSecTime, 
    const boost::optional<ComputerData>& params)
{
    if(params)
        return std::make_shared<Computer>(board, startingSecTime, *params);
    else
        return std::make_shared<Human>(board, startingSecTime);
}