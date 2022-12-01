#ifndef QTBOTWRAPPER_H
#define QTBOTWRAPPER_H

#include "engine/bot/base/aibotbase.h"

#include <vector>
#include <boost/optional.hpp>
#include <QTime>
#include <QThread>
#include <QObject>

/**********************************************************************************
 * Wrapping Aibot instances to propagate errors and update time by QTime objects  *
 **********************************************************************************/

class QtBotWrapper : public QObject
{
    Q_OBJECT
    friend class BoardDialog;
public:
    QtBotWrapper(AiBotBase* bot, const QTime& qtTimeTimeLeft);
    ~QtBotWrapper();
    void updateGame();
    QtBotWrapper(const AiBotBase&)=delete;
    QtBotWrapper& operator=(const AiBotBase&)=delete;

    void updateByOpponent(unsigned int moveIdx);

    boost::optional<std::string> getErrorMsg() const;

signals:
    void finished();

private slots:
    void updateGameSlot();

protected:
    boost::optional<std::string> errorMsg;
    AiBotBase* bot;

private:
    QThread thread;
    const QTime& qtTimeTimeLeft;

    std::chrono::milliseconds computeTimeLeft() const;
};

#endif // QTBOTWRAPPER_H
