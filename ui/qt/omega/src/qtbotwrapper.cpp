#include "qtbotwrapper.h"
#include <chrono>
#include <stdexcept>

QtBotWrapper::QtBotWrapper(AiBotBase* bot, const QTime& qtTimeTimeLeft):
    QObject(nullptr), // can not have a parent as it will be moved to QThread
    bot{bot},
    qtTimeTimeLeft{qtTimeTimeLeft}
{
    this->moveToThread(&thread);
    connect(&thread, SIGNAL (started()), this, SLOT (updateGameSlot()));
    connect(this, SIGNAL (finished()), &thread, SLOT (quit()));
}

QtBotWrapper::~QtBotWrapper()
{
    delete bot;
}

void QtBotWrapper::updateByOpponent(unsigned int moveIdx){
    bot->updateByOpponent(moveIdx);
}

void QtBotWrapper::updateGame(){
    thread.start();
}

void QtBotWrapper::updateGameSlot(){
    try{
        bot->setTimeLeft(computeTimeLeft());
        bot->updateGame();
    }
    catch(const std::exception& e){
        errorMsg = e.what();
    }
    catch(...){
        errorMsg = "Unknown error during execution.";
    }
    emit finished();
}
std::chrono::milliseconds QtBotWrapper::computeTimeLeft() const{
    int millisecs = 3.6e6 * qtTimeTimeLeft.hour() + 6e4 * qtTimeTimeLeft.minute() + 1e3 * qtTimeTimeLeft.second() + qtTimeTimeLeft.msec();
    return std::chrono::milliseconds(millisecs);
}

boost::optional<std::string> QtBotWrapper::getErrorMsg() const {
    return errorMsg;
}
