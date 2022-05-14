/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QLabel *modeLabel;
    QComboBox *modeComboBox;
    QLabel *boardLabel;
    QSpinBox *boardSizeSpinBox;
    QLabel *colorLabel;
    QComboBox *colorComboBox;
    QLabel *timeLabel;
    QScrollBar *timeSlider;
    QLabel *engineLabel;
    QComboBox *engineComboBox;
    QLabel *nodeLabel;
    QComboBox *nodeComboBox;
    QLabel *memoryLabel;
    QComboBox *memoryComboBox;
    QLabel *nodeLimitLabel;
    QScrollBar *nodeLimitSlider;
    QPushButton *startButton;
    QPushButton *quitButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(300, 486);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMaximumSize(QSize(300, 486));
        MainWindow->setStyleSheet(QLatin1String("QMainWindow{\n"
"        background-color: #444444;\n"
"}"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setStyleSheet(QStringLiteral(""));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        modeLabel = new QLabel(centralWidget);
        modeLabel->setObjectName(QStringLiteral("modeLabel"));

        verticalLayout->addWidget(modeLabel);

        modeComboBox = new QComboBox(centralWidget);
        modeComboBox->setObjectName(QStringLiteral("modeComboBox"));
        modeComboBox->setStyleSheet(QLatin1String("background-color: rgb(121, 121, 121);\n"
"selection-background-color: rgb(32, 105, 224);\n"
""));

        verticalLayout->addWidget(modeComboBox);

        boardLabel = new QLabel(centralWidget);
        boardLabel->setObjectName(QStringLiteral("boardLabel"));

        verticalLayout->addWidget(boardLabel);

        boardSizeSpinBox = new QSpinBox(centralWidget);
        boardSizeSpinBox->setObjectName(QStringLiteral("boardSizeSpinBox"));
        boardSizeSpinBox->setStyleSheet(QLatin1String("QSpinBox{\n"
"        background-color: #797979;\n"
"}"));

        verticalLayout->addWidget(boardSizeSpinBox);

        colorLabel = new QLabel(centralWidget);
        colorLabel->setObjectName(QStringLiteral("colorLabel"));

        verticalLayout->addWidget(colorLabel);

        colorComboBox = new QComboBox(centralWidget);
        colorComboBox->setObjectName(QStringLiteral("colorComboBox"));
        colorComboBox->setStyleSheet(QLatin1String("background-color: rgb(121, 121, 121);\n"
"selection-background-color: rgb(32, 105, 224);\n"
""));

        verticalLayout->addWidget(colorComboBox);

        timeLabel = new QLabel(centralWidget);
        timeLabel->setObjectName(QStringLiteral("timeLabel"));

        verticalLayout->addWidget(timeLabel);

        timeSlider = new QScrollBar(centralWidget);
        timeSlider->setObjectName(QStringLiteral("timeSlider"));
        timeSlider->setStyleSheet(QLatin1String("background-color: rgb(121, 121, 121);\n"
"selection-color: rgb(32, 105, 224);"));
        timeSlider->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(timeSlider);

        engineLabel = new QLabel(centralWidget);
        engineLabel->setObjectName(QStringLiteral("engineLabel"));

        verticalLayout->addWidget(engineLabel);

        engineComboBox = new QComboBox(centralWidget);
        engineComboBox->setObjectName(QStringLiteral("engineComboBox"));
        engineComboBox->setStyleSheet(QLatin1String("background-color: rgb(121, 121, 121);\n"
"selection-color: rgb(32, 105, 224);"));

        verticalLayout->addWidget(engineComboBox);

        nodeLabel = new QLabel(centralWidget);
        nodeLabel->setObjectName(QStringLiteral("nodeLabel"));

        verticalLayout->addWidget(nodeLabel);

        nodeComboBox = new QComboBox(centralWidget);
        nodeComboBox->setObjectName(QStringLiteral("nodeComboBox"));
        nodeComboBox->setStyleSheet(QLatin1String("background-color: rgb(121, 121, 121);\n"
"selection-color: rgb(32, 105, 224);"));

        verticalLayout->addWidget(nodeComboBox);

        memoryLabel = new QLabel(centralWidget);
        memoryLabel->setObjectName(QStringLiteral("memoryLabel"));

        verticalLayout->addWidget(memoryLabel);

        memoryComboBox = new QComboBox(centralWidget);
        memoryComboBox->setObjectName(QStringLiteral("memoryComboBox"));
        memoryComboBox->setMaximumSize(QSize(16777215, 16777215));
        memoryComboBox->setStyleSheet(QLatin1String("background-color: rgb(121, 121, 121);\n"
"selection-color: rgb(32, 105, 224);"));

        verticalLayout->addWidget(memoryComboBox);

        nodeLimitLabel = new QLabel(centralWidget);
        nodeLimitLabel->setObjectName(QStringLiteral("nodeLimitLabel"));

        verticalLayout->addWidget(nodeLimitLabel);

        nodeLimitSlider = new QScrollBar(centralWidget);
        nodeLimitSlider->setObjectName(QStringLiteral("nodeLimitSlider"));
        nodeLimitSlider->setStyleSheet(QLatin1String("background-color: rgb(121, 121, 121);\n"
"selection-color: rgb(32, 105, 224);"));
        nodeLimitSlider->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(nodeLimitSlider);

        startButton = new QPushButton(centralWidget);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setStyleSheet(QLatin1String("QPushButton{\n"
"        background-color: #797979;\n"
"}\n"
"\n"
"QPushButton:pressed{\n"
"        background-color: #2069e0 ;\n"
"}"));

        verticalLayout->addWidget(startButton);

        quitButton = new QPushButton(centralWidget);
        quitButton->setObjectName(QStringLiteral("quitButton"));
        quitButton->setStyleSheet(QLatin1String("QPushButton{\n"
"        background-color: #797979;\n"
"}\n"
"\n"
"QPushButton:pressed{\n"
"        background-color: #2069e0 ;\n"
"}"));

        verticalLayout->addWidget(quitButton);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        modeLabel->setText(QApplication::translate("MainWindow", "Game Mode", Q_NULLPTR));
        boardLabel->setText(QApplication::translate("MainWindow", "Board Size", Q_NULLPTR));
        colorLabel->setText(QApplication::translate("MainWindow", "Color", Q_NULLPTR));
        timeLabel->setText(QApplication::translate("MainWindow", "Time limit: 1 min", Q_NULLPTR));
        engineLabel->setText(QApplication::translate("MainWindow", "Search Engine", Q_NULLPTR));
        nodeLabel->setText(QApplication::translate("MainWindow", "Node Type", Q_NULLPTR));
        memoryLabel->setText(QApplication::translate("MainWindow", "Memory Management", Q_NULLPTR));
        nodeLimitLabel->setText(QApplication::translate("MainWindow", "Node Limit", Q_NULLPTR));
        startButton->setText(QApplication::translate("MainWindow", "Start", Q_NULLPTR));
        quitButton->setText(QApplication::translate("MainWindow", "Quit", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
