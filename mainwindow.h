#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scheduler.h"
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_fcfsRb_clicked();

    void on_sjfRb_clicked();

    void on_rrRb_clicked();

    void on_priorityRb_clicked();

    void on_nextButton_clicked();

    void on_confirmButton_clicked();

    void on_prevButton_clicked();

    void on_resetButton_clicked();

private:
    Ui::MainWindow *ui;
    int idCount;
    int i;
    double quantum;
    bool pre;
    bool nonPre;
    vector<Process> targetProcesses;
    vector<TimeSlot> timeSlots;
    vector<QLabel*> labels;
    vector<vector<QString>> history;

    void schedule();
    bool checkValidity(bool *isPriorityOk);
};
#endif // MAINWINDOW_H
