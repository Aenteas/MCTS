#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>

#include "boarddialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(double padding, double radius);
    ~MainWindow();
    MainWindow(const MainWindow&)=delete;
    MainWindow& operator=(const MainWindow&)=delete;

private slots:
    void on_startButton_clicked();

    void on_modeComboBox_currentTextChanged(const QString &text);

    void on_timeSlider_sliderMoved(int position);

    void on_memoryComboBox_currentTextChanged(const QString &text);

    void on_engineComboBox_currentTextChanged(const QString &text);

    void on_nodeLimitSlider_sliderMoved(int position);

public slots:
    void back_to_main();

private:
    void setNodeLimitLabel(int position);
    Ui::MainWindow* ui;
    BoardDialog* board=nullptr;
    double padding;
    double radius;
};

#endif // MAINWINDOW_H
