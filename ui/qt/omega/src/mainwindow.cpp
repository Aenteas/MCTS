#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
using namespace std;

// did not find a clean way of doing it in qt without code duplication
#define TIMESLIDERSLOT(postfix)                                                             \
void MainWindow::on_timeSlider ## postfix ## _sliderMoved(int position)                     \
{                                                                                           \
    QString str = QString("Time limit: %1 min").arg(position - 3);                          \
    if(position == 1)                                                                       \
        str = QString("Time limit: 15 sec");                                                \
    else if(position == 2)                                                                  \
        str = QString("Time limit: 30 sec");                                                \
    else if(position == 3)                                                                  \
        str = QString("Time limit: 45 sec");                                                \
    ui->timeLabel ## postfix->setText(str);                                                 \
}                                                                                           \

#define NODESLIDERSLOT(index)                                                               \
void MainWindow::setNodeLimitLabel ## index(int position){                                  \
    int value = position < 5 ? 2000 * position : (position - 4) * 10000;                    \
    ui->nodeLimitLabel ## index->setText(QString("Node limit: %1").arg(value));             \
}                                                                                           \

TIMESLIDERSLOT(1)
TIMESLIDERSLOT(2)
TIMESLIDERSLOT(Main)

NODESLIDERSLOT(1)
NODESLIDERSLOT(2)

#define SLOTS(index)                                                                        \
void MainWindow::on_memoryComboBox ## index ## _currentTextChanged(const QString &text){    \
    if(text == QString("node recycling")){                                                  \
        ui->nodeLimitSlider ## index->show();                                               \
        ui->nodeLimitLabel ## index->show();                                                \
    }                                                                                       \
    else{                                                                                   \
        ui->nodeLimitSlider ## index->hide();                                               \
        ui->nodeLimitLabel ## index->hide();                                                \
    }                                                                                       \
}                                                                                           \
void MainWindow::on_engineComboBox ## index ## _currentTextChanged(const QString &text){    \
    if(text == QString("random")){                                                          \
        ui->memoryComboBox ## index->hide();                                                \
        ui->memoryLabel ## index->hide();                                                   \
        ui->nodeLimitSlider ## index->hide();                                               \
        ui->nodeLimitLabel ## index->hide();                                                \
        ui->nodeComboBox ## index->hide();                                                  \
        ui->policyComboBox ## index->hide();                                                \
        ui->nodeLabel ## index->hide();                                                     \
        ui->policyLabel ## index->hide();                                                   \
        if(ui->modeComboBox->currentText() == "CompvComp" &&                                \
           ui->engineComboBox1->currentText() == "random" &&                                \
           ui->engineComboBox2->currentText() == "random"){                                 \
            ui->botWidget->setMinimumHeight(150);                                           \
            ui->botWidget->setMaximumHeight(150);                                           \
            setMinimumHeight(400);                                                          \
            setMaximumHeight(400);                                                          \
        }                                                                                   \
        else if(ui->modeComboBox->currentText() == "PvComp"){                               \
            if((ui->engineComboBox1->currentText() == "random" &&                           \
                ui->colorComboBox->currentText() == "White") ||                             \
                (ui->engineComboBox2->currentText() == "random" &&                          \
                ui->colorComboBox->currentText() == "Black")){                              \
                ui->botWidget->setMinimumHeight(150);                                       \
                ui->botWidget->setMaximumHeight(150);                                       \
                setMinimumHeight(500);                                                      \
                setMaximumHeight(500);                                                      \
            }                                                                               \
            else{                                                                           \
                ui->botWidget->setMinimumHeight(400);                                       \
                ui->botWidget->setMaximumHeight(400);                                       \
                setMinimumHeight(650);                                                      \
                setMaximumHeight(650);                                                      \
            }                                                                               \
        }                                                                                   \
    }                                                                                       \
    else if(text == QString("MCTS")){                                                       \
        ui->memoryComboBox ## index->show();                                                \
        ui->memoryLabel ## index->show();                                                   \
        ui->nodeComboBox ## index->show();                                                  \
        ui->policyComboBox ## index->show();                                                \
        ui->nodeLabel ## index->show();                                                     \
        ui->policyLabel ## index->show();                                                   \
        if(ui->memoryComboBox ## index->currentText() == "node recycling"){                 \
            ui->nodeLimitSlider ## index->show();                                           \
            ui->nodeLimitLabel ## index->show();                                            \
        }                                                                                   \
        setMinimumHeight(700);                                                              \
        setMaximumHeight(700);                                                              \
        ui->botWidget->setMinimumHeight(400);                                               \
        ui->botWidget->setMaximumHeight(400);                                               \
    }                                                                                       \
}                                                                                           \
void MainWindow::on_nodeLimitSlider ## index ## _sliderMoved(int position){                 \
    setNodeLimitLabel ## index(position);                                                   \
}                                                                                           \

SLOTS(1)
SLOTS(2)

#define GETSECSFROMTIMESLIDER(postfix)                                                      \
unsigned MainWindow::getSecsFromTimeSlider ## postfix() const{                              \
    unsigned pos = ui->timeSlider ## postfix->value();                                      \
    return pos < 5 ? pos * 15 : (pos - 3) * 60;                                             \
}                                                                                           \

GETSECSFROMTIMESLIDER(1)
GETSECSFROMTIMESLIDER(2)
GETSECSFROMTIMESLIDER(Main)

MainWindow::MainWindow(double padding, double radius) :
    QMainWindow(nullptr),
    ui(make_shared<Ui::MainWindow>()),
    padding{padding},
    radius{radius},
    board(nullptr)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Omega"));

    // computer settings
    // white
    ui->timeSlider1->setRange(1, 13);
    ui->timeSlider1->setValue(4);

    ui->memoryComboBox1->addItem(QString("standard"));
    ui->memoryComboBox1->addItem(QString("node recycling"));
    ui->memoryComboBox1->setCurrentIndex(1);

    ui->engineComboBox1->addItem(QString("random"));
    ui->engineComboBox1->addItem(QString("MCTS"));
    ui->engineComboBox1->setCurrentIndex(0);

    ui->nodeComboBox1->addItem(QString("RAVE"));
    ui->nodeComboBox1->addItem(QString("UCT-2"));
    ui->nodeComboBox1->setCurrentIndex(0);

    ui->policyComboBox1->addItem(QString("random"));
    ui->policyComboBox1->addItem(QString("MAST"));
    ui->policyComboBox1->setCurrentIndex(1);

    ui->nodeLimitSlider1->setRange(1, 9);
    ui->nodeLimitSlider1->setSingleStep(1);
    ui->nodeLimitSlider1->setValue(5);

    setNodeLimitLabel1(5);

    // black
    ui->timeSlider2->setRange(1, 13);
    ui->timeSlider2->setValue(4);

    ui->memoryComboBox2->addItem(QString("standard"));
    ui->memoryComboBox2->addItem(QString("node recycling"));
    ui->memoryComboBox2->setCurrentIndex(1);

    ui->engineComboBox2->addItem(QString("random"));
    ui->engineComboBox2->addItem(QString("MCTS"));
    ui->engineComboBox2->setCurrentIndex(0);

    ui->nodeComboBox2->addItem(QString("RAVE"));
    ui->nodeComboBox2->addItem(QString("UCT-2"));
    ui->nodeComboBox2->setCurrentIndex(0);

    ui->policyComboBox2->addItem(QString("random"));
    ui->policyComboBox2->addItem(QString("MAST"));
    ui->policyComboBox2->setCurrentIndex(1);

    ui->nodeLimitSlider2->setRange(1, 9);
    ui->nodeLimitSlider2->setSingleStep(1);
    ui->nodeLimitSlider2->setValue(5);

    setNodeLimitLabel2(5);

    // main part
    ui->boardSizeSpinBox->setRange(3, 10);
    ui->boardSizeSpinBox->setValue(3);

    ui->colorComboBox->addItem(QString("White"));
    ui->colorComboBox->addItem(QString("Black"));

    ui->modeComboBox->addItem(QString("PvP"));
    ui->modeComboBox->addItem(QString("PvComp"));
    ui->modeComboBox->addItem(QString("CompvComp"));
    ui->modeComboBox->setCurrentIndex(0);

    ui->timeSliderMain->setRange(1, 13);
    ui->timeSliderMain->setValue(4);

    connect(ui->quitButton, SIGNAL (clicked()), QApplication::instance(), SLOT (quit()));
}

void MainWindow::on_startButton_clicked()
{
    QString mode = ui->modeComboBox->currentText();
    int boardSize = ui->boardSizeSpinBox->value();
    boost::optional<BoardDialog::ComputerData> params1 = boost::none;
    boost::optional<BoardDialog::ComputerData> params2 = boost::none;
    unsigned time1, time2, from;

    #define CREATECOMPUTER(index)                                                                   \
        time ## index = getSecsFromTimeSlider ## index();                                           \
        QString version ## index = ui->engineComboBox ## index->currentText();                      \
        bool recycling ## index = ui->memoryComboBox ## index->currentText() == "node recycling";   \
        QString node ## index = ui->nodeComboBox ## index->currentText();                           \
        QString policy ## index = ui->policyComboBox ## index->currentText();                       \
        from = ui->nodeLimitLabel ## index->text().indexOf(":")+1;                                  \
        unsigned budget ## index = ui->nodeLimitLabel ## index->text().mid(from).toLong();          \
        params ## index = boost::optional<BoardDialog::ComputerData>({version ## index,             \
            node ## index,                                                                          \
            policy ## index,                                                                        \
            recycling ## index,                                                                     \
            budget ## index});                                                                      \
    
    if(mode == "PvP"){
        time1 = time2 = getSecsFromTimeSliderMain();
    }
    else if(mode == "PvComp"){
        QString color = ui->colorComboBox->currentText();
        if(color == "WHITE"){
            time1 = getSecsFromTimeSliderMain();

            // black player is computer
            CREATECOMPUTER(2)
        }
        else{
            time2 = getSecsFromTimeSliderMain();

            // white player is computer
            CREATECOMPUTER(1)
        }
    }
    else{ // CompvComp
        // white player
        CREATECOMPUTER(1)

        // black player
        CREATECOMPUTER(2)
    }

    try{
        board = make_shared<BoardDialog>(this, boardSize, radius, padding, time1, time2, params1, params2);
    }
    catch(const std::exception& e){
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
        return;
    }
    catch(...){
        QMessageBox msgBox;
        msgBox.setText("Unknown error during board dialog creation.");
        msgBox.exec();
        return;
    }
    hide();
    board->show();
}

void MainWindow::on_modeComboBox_currentTextChanged(const QString &text)
{
    // hide other options
    if(text == QString("PvP")){
        setMinimumHeight(300);
        setMaximumHeight(300);
        ui->colorLabel->hide();
        ui->colorComboBox->hide();
        ui->botWidget->hide();
        ui->timeLabelMain->show();
        ui->timeSliderMain->show();
    }
    else if(text == QString("PvComp")){
        ui->colorLabel->show();
        ui->colorComboBox->show();
        ui->botWidget->show();
        ui->timeLabelMain->show();
        ui->timeSliderMain->show();
        if(ui->colorComboBox->currentText() == QString("White")){
            if(ui->engineComboBox2->currentText() == "random"){
                setMinimumHeight(500);
                setMaximumHeight(500);
                ui->botWidget->setMinimumHeight(150);
                ui->botWidget->setMaximumHeight(150);
            }
            else{
                setMinimumHeight(700);
                setMaximumHeight(650);
                ui->botWidget->setMinimumHeight(400);
                ui->botWidget->setMaximumHeight(400);
            }
            ui->botWidget2->show();
            ui->botWidget1->hide();
        }
        else{
            if(ui->engineComboBox1->currentText() == "random"){
                setMinimumHeight(500);
                setMaximumHeight(500);
                ui->botWidget->setMinimumHeight(150);
                ui->botWidget->setMaximumHeight(150);
            }
            else{
                setMinimumHeight(700);
                setMaximumHeight(700);
                ui->botWidget->setMinimumHeight(400);
                ui->botWidget->setMaximumHeight(400);
            }
            ui->botWidget1->show();
            ui->botWidget2->hide();
        }
    }
    else{ // CompvComp
        if(ui->engineComboBox1->currentText() == "random" && ui->engineComboBox2->currentText() == "random"){
            setMinimumHeight(400);
            setMaximumHeight(400);
            ui->botWidget->setMinimumHeight(150);
            ui->botWidget->setMaximumHeight(150);
        }
        else{
            setMinimumHeight(650);
            setMaximumHeight(650);
            ui->botWidget->setMinimumHeight(400);
            ui->botWidget->setMaximumHeight(400);
        }
        ui->colorLabel->hide();
        ui->colorComboBox->hide();
        ui->timeLabelMain->hide();
        ui->timeSliderMain->hide();
        ui->botWidget->show();
        ui->botWidget1->show();
        ui->botWidget2->show();
    }
}

void MainWindow::on_colorComboBox_currentTextChanged(const QString &text)
{
    if(text == QString("White")){
        ui->botWidget2->show();
        ui->botWidget1->hide();
        if(ui->engineComboBox2->currentText() == "random"){
            ui->botWidget->setMinimumHeight(150);
            ui->botWidget->setMaximumHeight(150);
            setMinimumHeight(500);
            setMaximumHeight(500);
        }
        else{
            ui->botWidget->setMinimumHeight(400);
            ui->botWidget->setMaximumHeight(400);
            setMinimumHeight(700);
            setMaximumHeight(700);
        }
    }
    else{
        ui->botWidget1->show();
        ui->botWidget2->hide();
        if(ui->engineComboBox1->currentText() == "random"){
            ui->botWidget->setMinimumHeight(150);
            ui->botWidget->setMaximumHeight(150);
            setMinimumHeight(500);
            setMaximumHeight(500);
        }
        else{
            ui->botWidget->setMinimumHeight(400);
            ui->botWidget->setMaximumHeight(400);
            setMinimumHeight(700);
            setMaximumHeight(700);
        }
    }
}

void MainWindow::back_to_main(){
    if(board){
        board->close();
        board = nullptr;
    }
    show();
}