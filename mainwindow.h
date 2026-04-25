#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QPushButton>
#include "scheduler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnStart_clicked();
    void on_btnPause_clicked();
    void on_btnAdd_clicked();
    void on_chkAutoGen_stateChanged(int arg1);
    void on_sliderSpeed_valueChanged(int value);
    void on_comboAlgo_currentIndexChanged(int index);
    void on_btnClear_clicked();
    void on_btnToggleMutex_clicked();
    void on_btnToggleSemaphore_clicked();
    void on_btnDeadlock_clicked(); // <-- Only declared ONCE now!
    
    void spawnVehicle();
    void updateSlotUI(int index, bool occupied, int vehicleId);
    void updateQueues(QStringList readyList, QStringList waitingList, QString runningStr);
    void appendLog(QString msg);
    void drawGanttBlock(int id, int duration, qint64 wt, qint64 tat);
    void updatePerformanceGraph(int readyCount, int waitCount, int runningCount);
    void updateMetrics(int total, int completed, double avgWT, double avgTAT, double tp);
    void updateDebug(int semValue, bool mutexLocked, int blockedCount);
    void triggerDeadlockWarning();

private:
    Ui::MainWindow *ui;
    Scheduler *scheduler;
    QGraphicsScene *ganttScene;
    QList<QPushButton*> slotButtons;
    
    QLineSeries *readySeries;
    QLineSeries *waitingSeries;
    QLineSeries *runningSeries;
    QChart *chart;
    
    int timeStep;
    int vehicleCounter;
    int currentGanttX;
};
#endif
