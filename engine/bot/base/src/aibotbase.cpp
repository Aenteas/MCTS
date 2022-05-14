#include "aibotbase.h"

void AiBotBase::startTimer() { start = std::chrono::steady_clock::now(); }

void AiBotBase::setTimeLeft(const std::chrono::milliseconds& timeLeft) { this->timeLeft = timeLeft; }

bool AiBotBase::isTimeOut() const{
    auto end = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return elapsed_time > timeLeft;
}
