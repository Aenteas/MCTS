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
    ~MainWindow()=default;
    MainWindow(const MainWindow&)=delete;
    MainWindow& operator=(const MainWindow&)=delete;

private slots:
    void on_startButton_clicked();

    void on_modeComboBox_currentTextChanged(const QString &text);
    void on_colorComboBox_currentTextChanged(const QString &text);

    void on_timeSlider1_sliderMoved(int position);
    void on_timeSlider2_sliderMoved(int position);
    void on_timeSliderMain_sliderMoved(int position);

    void setNodeLimitLabel1(int position);
    void setNodeLimitLabel2(int position);

    void on_memoryComboBox1_currentTextChanged(const QString &text);
    void on_engineComboBox1_currentTextChanged(const QString &text);
    void on_nodeLimitSlider1_sliderMoved(int position);

    void on_memoryComboBox2_currentTextChanged(const QString &text);
    void on_engineComboBox2_currentTextChanged(const QString &text);
    void on_nodeLimitSlider2_sliderMoved(int position);

public slots:
    void back_to_main();

private:
    void setNodeLimitLabel(QLabel* label, int position);
    
    unsigned getSecsFromTimeSlider1() const;
    unsigned getSecsFromTimeSlider2() const;
    unsigned getSecsFromTimeSliderMain() const;

    std::shared_ptr<Ui::MainWindow> ui;
    std::shared_ptr<BoardDialog> board;
    double padding;
    double radius;
    inline static std::unordered_map<unsigned, unsigned> nodeBudgetMap = {
        {1, 20},
        {2, 50},
        {3, 100},
        {4, 200},
        {5, 500},
        {6, 1000},
        {7, 2000},
        {8, 5000},
        {9, 10000}
    };
};

#endif // MAINWINDOW_H
