#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(double padding, double radius) :
    QMainWindow(nullptr),
    ui(new Ui::MainWindow),
    padding{padding},
    radius{radius}
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Omega"));
    ui->boardSizeSpinBox->setRange(3, 10);
    ui->boardSizeSpinBox->setValue(3);

    ui->colorComboBox->addItem(QString("White"));
    ui->colorComboBox->addItem(QString("Black"));

    ui->modeComboBox->addItem(QString("PvP"));
    ui->modeComboBox->addItem(QString("vs AI"));
    ui->modeComboBox->setCurrentIndex(0);

    ui->timeSlider->setRange(1, 10);
    ui->timeSlider->setValue(1);

    ui->engineComboBox->addItem(QString("Random"));
    ui->engineComboBox->addItem(QString("MCTS"));
    ui->engineComboBox->setCurrentIndex(0);

    ui->nodeComboBox->addItem(QString("RAVE"));
    ui->nodeComboBox->addItem(QString("UCT-2"));
    ui->nodeComboBox->setCurrentIndex(0);

    ui->policyComboBox->addItem(QString("random"));
    ui->policyComboBox->addItem(QString("MAST"));
    ui->policyComboBox->setCurrentIndex(0);

    ui->memoryComboBox->addItem(QString("OneDepthVNew"));
    ui->memoryComboBox->addItem(QString("Node Recycling"));
    ui->memoryComboBox->setCurrentIndex(0);

    ui->nodeLimitSlider->setRange(1, 10);
    ui->nodeLimitSlider->setSingleStep(1);
    ui->nodeLimitSlider->setValue(10);

    setNodeLimitLabel(10);

    ui->colorComboBox->hide();
    ui->colorLabel->hide();
    ui->memoryComboBox->hide();
    ui->memoryLabel->hide();
    ui->engineComboBox->hide();
    ui->engineLabel->hide();
    ui->nodeComboBox->hide();
    ui->policyComboBox->hide();
    ui->nodeLabel->hide();
    ui->policyLabel->hide();
    ui->nodeLimitSlider->hide();
    ui->nodeLimitLabel->hide();
    ui->timeSlider->hide();
    ui->timeLabel->hide();
    board = nullptr;

    setMaximumHeight(220);

    connect(ui->quitButton, SIGNAL (clicked()), QApplication::instance(), SLOT (quit()));
}

MainWindow::~MainWindow()
{
    delete ui;
    if(board)
        delete board;
}

void MainWindow::on_startButton_clicked()
{
    QString mode = ui->modeComboBox->currentText();
    int boardSize = ui->boardSizeSpinBox->value();
    QString color = ui->colorComboBox->currentText();
    unsigned int time = ui->timeSlider->value();
    QString bot = ui->engineComboBox->currentText();
    bool recycling = ui->memoryComboBox->currentText() == "Node Recycling";
    QString node = ui->nodeComboBox->currentText();
    QString policy = ui->policyComboBox->currentText();
    unsigned int from = ui->nodeLimitLabel->text().indexOf(":")+1;
    unsigned int budget = ui->nodeLimitLabel->text().mid(from).toLong();

    try{
        board = new BoardDialog(this, boardSize, radius, padding, mode, color, time, bot, node, policy, recycling, budget);
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
        ui->colorComboBox->hide();
        ui->colorLabel->hide();
        ui->memoryComboBox->hide();
        ui->memoryLabel->hide();
        ui->engineComboBox->hide();
        ui->engineLabel->hide();
        ui->nodeComboBox->hide();
        ui->policyComboBox->hide();
        ui->nodeLabel->hide();
        ui->policyLabel->hide();
        ui->nodeLimitSlider->hide();
        ui->nodeLimitLabel->hide();
        ui->timeSlider->hide();
        ui->timeLabel->hide();
        setMaximumHeight(220);
    }
    // show additional options
    else{
        ui->colorLabel->show();
        ui->colorComboBox->show();
        ui->engineComboBox->show();
        ui->engineLabel->show();
        ui->timeSlider->show();
        ui->timeLabel->show();
        setMaximumHeight(350);
    }
}

void MainWindow::on_engineComboBox_currentTextChanged(const QString &text)
{
    // hide other options
    if(text == QString("Random")){
        ui->memoryComboBox->hide();
        ui->memoryLabel->hide();
        ui->nodeLimitSlider->hide();
        ui->nodeLimitLabel->hide();
        ui->nodeComboBox->hide();
        ui->policyComboBox->hide();
        ui->nodeLabel->hide();
        ui->policyLabel->hide();
        setMaximumHeight(400);
    }
    // show additional options
    else if(text == QString("MCTS")){
        ui->memoryComboBox->show();
        ui->memoryLabel->show();
        ui->nodeComboBox->show();
        ui->policyComboBox->show();
        ui->nodeLabel->show();
        ui->policyLabel->show();
        if(ui->memoryComboBox->currentText() == "Node Recycling"){
            ui->nodeLimitSlider->show();
            ui->nodeLimitLabel->show();
            setMaximumHeight(500);
        }
        else
            setMaximumHeight(450);
    }
}

void MainWindow::on_memoryComboBox_currentTextChanged(const QString &text)
{
    if(text == QString("Node Recycling")){
        ui->nodeLimitSlider->show();
        ui->nodeLimitLabel->show();
        setMaximumHeight(500);
    }
    else{
        ui->nodeLimitSlider->hide();
        ui->nodeLimitLabel->hide();
        setMaximumHeight(450);
    }
}

void MainWindow::back_to_main(){
    if(board){
        board->close();
        delete board;
        board = nullptr;
    }
    show();
}

void MainWindow::on_timeSlider_sliderMoved(int position)
{
    ui->timeLabel->setText(QString("Time limit: %1 min").arg(position));
}


void MainWindow::on_nodeLimitSlider_sliderMoved(int position)
{
    setNodeLimitLabel(position);
}

void MainWindow::setNodeLimitLabel(int position){
    int value = position * 10000;
    ui->nodeLimitLabel->setText(QString("Node limit: %1").arg(value));
}
