/********************************************************************************
** Form generated from reading UI file 'boarddialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BOARDDIALOG_H
#define UI_BOARDDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_BoardDialog
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QFrame *controlPanel;
    QGridLayout *gridLayout_3;
    QGridLayout *controlPanelLayout;
    QLCDNumber *blackScore;
    QLCDNumber *whiteScore;
    QSpacerItem *verticalSpacer;
    QPushButton *startButton;
    QFrame *black;
    QPushButton *quitButton;
    QFrame *white;
    QLabel *description;
    QLabel *whiteCountDown;
    QLabel *blackCountDown;
    QSpacerItem *horizontalSpacer;

    void setupUi(QDialog *BoardDialog)
    {
        if (BoardDialog->objectName().isEmpty())
            BoardDialog->setObjectName(QStringLiteral("BoardDialog"));
        BoardDialog->resize(400, 300);
        BoardDialog->setMaximumSize(QSize(1150, 600));
        BoardDialog->setStyleSheet(QLatin1String("QDialog{\n"
"        background-color: #444444;\n"
"}"));
        gridLayout = new QGridLayout(BoardDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        controlPanel = new QFrame(BoardDialog);
        controlPanel->setObjectName(QStringLiteral("controlPanel"));
        controlPanel->setMinimumSize(QSize(160, 0));
        controlPanel->setMaximumSize(QSize(360, 1000));
        controlPanel->setFrameShape(QFrame::StyledPanel);
        controlPanel->setFrameShadow(QFrame::Raised);
        gridLayout_3 = new QGridLayout(controlPanel);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        controlPanelLayout = new QGridLayout();
        controlPanelLayout->setObjectName(QStringLiteral("controlPanelLayout"));
        blackScore = new QLCDNumber(controlPanel);
        blackScore->setObjectName(QStringLiteral("blackScore"));
        blackScore->setMinimumSize(QSize(50, 23));
        blackScore->setStyleSheet(QLatin1String("QLCDNumber{\n"
"        background-color: gray;\n"
"        padding: 5px;\n"
"        color: #ffffff;\n"
"}\n"
""));

        controlPanelLayout->addWidget(blackScore, 1, 2, 1, 1);

        whiteScore = new QLCDNumber(controlPanel);
        whiteScore->setObjectName(QStringLiteral("whiteScore"));
        whiteScore->setStyleSheet(QLatin1String("QLCDNumber{\n"
"        background-color: gray;\n"
"        padding: 5px;\n"
"        color: #ffffff;\n"
"}\n"
""));

        controlPanelLayout->addWidget(whiteScore, 0, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        controlPanelLayout->addItem(verticalSpacer, 3, 2, 1, 1);

        startButton = new QPushButton(controlPanel);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setFocusPolicy(Qt::NoFocus);
        startButton->setStyleSheet(QLatin1String("QPushButton{\n"
"        background-color: #797979;\n"
"    color: rgb(238, 238, 236);\n"
"}\n"
"\n"
"QPushButton:pressed{\n"
"        background-color: #2069e0 ;\n"
"   color: rgb(238, 238, 236);\n"
"}"));
        startButton->setFlat(false);

        controlPanelLayout->addWidget(startButton, 4, 0, 1, 3);

        black = new QFrame(controlPanel);
        black->setObjectName(QStringLiteral("black"));
        black->setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 0);"));
        black->setFrameShape(QFrame::StyledPanel);
        black->setFrameShadow(QFrame::Raised);

        controlPanelLayout->addWidget(black, 1, 0, 1, 1);

        quitButton = new QPushButton(controlPanel);
        quitButton->setObjectName(QStringLiteral("quitButton"));
        quitButton->setFocusPolicy(Qt::NoFocus);
        quitButton->setStyleSheet(QLatin1String("QPushButton{\n"
"        background-color: #797979;\n"
"    color: rgb(238, 238, 236);\n"
"}\n"
"\n"
"QPushButton:pressed{\n"
"        background-color: #2069e0 ;\n"
"   color: rgb(238, 238, 236);\n"
"}"));

        controlPanelLayout->addWidget(quitButton, 5, 0, 1, 3);

        white = new QFrame(controlPanel);
        white->setObjectName(QStringLiteral("white"));
        white->setMinimumSize(QSize(23, 23));
        white->setMaximumSize(QSize(23, 23));
        white->setStyleSheet(QStringLiteral("background-color: rgb(238, 238, 236);"));
        white->setFrameShape(QFrame::StyledPanel);
        white->setFrameShadow(QFrame::Raised);

        controlPanelLayout->addWidget(white, 0, 0, 1, 1);

        description = new QLabel(controlPanel);
        description->setObjectName(QStringLiteral("description"));
        description->setMinimumSize(QSize(0, 23));
        description->setStyleSheet(QLatin1String("background-color: rgb(121, 121, 121);\n"
"color: rgb(238, 238, 236);"));
        description->setAlignment(Qt::AlignCenter);

        controlPanelLayout->addWidget(description, 2, 0, 1, 3);

        whiteCountDown = new QLabel(controlPanel);
        whiteCountDown->setObjectName(QStringLiteral("whiteCountDown"));
        whiteCountDown->setMinimumSize(QSize(60, 0));
        whiteCountDown->setStyleSheet(QLatin1String("background-color: rgb(90,90,90);\n"
"color: rgb(238, 238, 236);"));
        whiteCountDown->setAlignment(Qt::AlignCenter);

        controlPanelLayout->addWidget(whiteCountDown, 0, 1, 1, 1);

        blackCountDown = new QLabel(controlPanel);
        blackCountDown->setObjectName(QStringLiteral("blackCountDown"));
        blackCountDown->setMinimumSize(QSize(60, 23));
        blackCountDown->setStyleSheet(QLatin1String("background-color: rgb(90,90,90);\n"
"color: rgb(238, 238, 236);"));
        blackCountDown->setAlignment(Qt::AlignCenter);

        controlPanelLayout->addWidget(blackCountDown, 1, 1, 1, 1);


        gridLayout_3->addLayout(controlPanelLayout, 0, 0, 1, 1);


        horizontalLayout->addWidget(controlPanel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);


        retranslateUi(BoardDialog);

        QMetaObject::connectSlotsByName(BoardDialog);
    } // setupUi

    void retranslateUi(QDialog *BoardDialog)
    {
        BoardDialog->setWindowTitle(QApplication::translate("BoardDialog", "Dialog", Q_NULLPTR));
        startButton->setText(QApplication::translate("BoardDialog", "Start Game", Q_NULLPTR));
        quitButton->setText(QApplication::translate("BoardDialog", "Back", Q_NULLPTR));
        description->setText(QApplication::translate("BoardDialog", " Welcome!", Q_NULLPTR));
        whiteCountDown->setText(QString());
        blackCountDown->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class BoardDialog: public Ui_BoardDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BOARDDIALOG_H
