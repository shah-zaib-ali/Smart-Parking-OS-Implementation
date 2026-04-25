#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QGraphicsRectItem>
#include <QtCharts/QValueAxis>
#include <QThread>
#include <QTimer>
#include <QMetaObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), vehicleCounter(1), currentGanttX(0), timeStep(0) {
    ui->setupUi(this);

    slotButtons = {ui->btnSlot1, ui->btnSlot2, ui->btnSlot3, ui->btnSlot4, ui->btnSlot5};
    for(auto btn : slotButtons) btn->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; border-radius: 5px;");

    scheduler = new Scheduler(this);
    connect(scheduler, &Scheduler::logMessage, this, &MainWindow::appendLog);
    connect(scheduler, &Scheduler::queuesUpdated, this, &MainWindow::updateQueues);
    connect(scheduler, &Scheduler::statsUpdated, this, &MainWindow::updatePerformanceGraph);
    connect(scheduler, &Scheduler::metricsUpdated, this, &MainWindow::updateMetrics);
    connect(scheduler, &Scheduler::debugUpdated, this, &MainWindow::updateDebug);
    connect(scheduler, &Scheduler::deadlockDetected, this, &MainWindow::triggerDeadlockWarning);

    // Enhanced Chart
    readySeries = new QLineSeries(); readySeries->setName("Ready Queue");
    waitingSeries = new QLineSeries(); waitingSeries->setName("Blocked/Waiting");
    runningSeries = new QLineSeries(); runningSeries->setName("Running");
    
    chart = new QChart();
    chart->addSeries(readySeries); 
    chart->addSeries(waitingSeries);
    chart->addSeries(runningSeries);
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, 20);
    chart->axes(Qt::Vertical).first()->setRange(0, 5); // Start small, scale up dynamically
    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    ganttScene = new QGraphicsScene(this);
    ui->ganttView->setScene(ganttScene);
    
    connect(scheduler->findChild<QTimer*>(""), &QTimer::timeout, this, [this](){
        if(ui->chkAutoGen->isChecked() && currentState == RUNNING) spawnVehicle();
    });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_btnStart_clicked() {
    currentState = RUNNING;
    ui->lblState->setText("System State: RUNNING");
    ui->lblState->setStyleSheet("color: green; font-weight: bold; padding: 5px;");
    appendLog("System Started.");
}

void MainWindow::on_btnPause_clicked() {
    if(currentState == RUNNING) {
        currentState = PAUSED;
        ui->lblState->setText("System State: PAUSED");
        ui->lblState->setStyleSheet("color: orange; font-weight: bold; padding: 5px;");
        appendLog("System Paused.");
    } else if (currentState == PAUSED) {
        currentState = RUNNING;
        ui->lblState->setText("System State: RUNNING");
        ui->lblState->setStyleSheet("color: green; font-weight: bold; padding: 5px;");
        appendLog("System Resumed.");
    }
}

void MainWindow::on_btnAdd_clicked() { spawnVehicle(); }
void MainWindow::on_chkAutoGen_stateChanged(int state) { scheduler->setAutoGenerate(state == Qt::Checked); }
void MainWindow::on_sliderSpeed_valueChanged(int value) { simulationSpeed = value; appendLog(QString("Speed set to %1x").arg(value)); }
void MainWindow::on_comboAlgo_currentIndexChanged(int index) { scheduler->setAlgorithm(static_cast<SchedulingAlgorithm>(index)); }
void MainWindow::on_btnClear_clicked() { ui->txtLog->clear(); }

void MainWindow::spawnVehicle() {
    int burstTime = QRandomGenerator::global()->bounded(2000, 6000);
    int priority = QRandomGenerator::global()->bounded(1, 10);
    VehicleThread *vt = new VehicleThread(vehicleCounter++, burstTime, priority, this);
    connect(vt, &VehicleThread::logMessage, this, &MainWindow::appendLog);
    connect(vt, &VehicleThread::slotUpdated, this, &MainWindow::updateSlotUI);
    connect(vt, &VehicleThread::executionFinished, this, &MainWindow::drawGanttBlock);
    scheduler->addVehicle(vt);
    vt->start();
}

void MainWindow::updateSlotUI(int index, bool occupied, int vehicleId) {
    if (occupied) {
        slotButtons[index]->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold; border-radius: 5px;");
        slotButtons[index]->setText(QString("V-%1").arg(vehicleId));
    } else {
        slotButtons[index]->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; border-radius: 5px;");
        slotButtons[index]->setText(QString("S%1").arg(index+1));
    }
}

void MainWindow::updateQueues(QStringList readyList, QStringList waitingList, QString runningStr) {
    ui->listReady->clear(); ui->listReady->addItems(readyList);
    ui->listWaiting->clear(); ui->listWaiting->addItems(waitingList);
    ui->lblRunning->setText(runningStr);
}

void MainWindow::appendLog(QString msg) { ui->txtLog->append(msg); }

void MainWindow::drawGanttBlock(int id, int duration, qint64 wt, qint64 tat) {
    int width = (duration / simulationSpeed) / 40;
    ganttScene->addRect(currentGanttX, 0, width, 40, QPen(Qt::black), QBrush(QColor(100, 200, 255)));
    QGraphicsTextItem *text = ganttScene->addText(QString("V%1").arg(id));
    text->setPos(currentGanttX + 2, 10);
    currentGanttX += width + 2;
    ui->ganttView->setSceneRect(0, 0, currentGanttX + 50, 60);
}

void MainWindow::updatePerformanceGraph(int readyCount, int waitCount, int runningCount) {
    timeStep++;
    readySeries->append(timeStep, readyCount);
    waitingSeries->append(timeStep, waitCount);
    runningSeries->append(timeStep, runningCount);

    QValueAxis *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());

    if (axisX && timeStep > 20) {
        axisX->setRange(timeStep - 20, timeStep);
    }

    int maxY = std::max({readyCount, waitCount, runningCount});
    if (axisY && maxY >= axisY->max()) {
        axisY->setRange(0, maxY + 3);
    }
}

void MainWindow::updateMetrics(int total, int completed, double avgWT, double avgTAT, double tp) {
    ui->lblMetrics->setText(QString("Created: %1 | Completed: %2 | Avg WT: %3ms | Avg TAT: %4ms | Throughput: %5/s")
                            .arg(total).arg(completed).arg(avgWT, 0, 'f', 1).arg(avgTAT, 0, 'f', 1).arg(tp, 0, 'f', 2));
}

void MainWindow::updateDebug(int semValue, bool mutexLocked, int blockedCount) {
    ui->lblSem->setText(QString("Semaphore Available: %1").arg(semValue));
    ui->lblMutex->setText(QString("Mutex Locked: %1").arg(mutexLocked ? "TRUE" : "FALSE"));
    ui->lblBlocked->setText(QString("Blocked Threads: %1").arg(blockedCount));
    
    if (semValue > 0 && currentState == RUNNING) {
        ui->lblState->setText("System State: RUNNING");
        ui->lblState->setStyleSheet("color: green; font-weight: bold; padding: 5px;");
    }
}

void MainWindow::triggerDeadlockWarning() {
    ui->lblState->setText("🚨 SYSTEM OVERLOAD / WAITING 🚨");
    ui->lblState->setStyleSheet("background-color: red; color: white; font-weight: bold; padding: 5px; border-radius: 3px;");
    appendLog("🚨 OS WARNING: Resource Starvation Detected! Many threads blocked.");
}

void MainWindow::on_btnToggleMutex_clicked() {
    mutexEnabled = !mutexEnabled;
    if (mutexEnabled) {
        ui->btnToggleMutex->setText("🟢 Mutex ON");
        ui->btnToggleMutex->setStyleSheet("background-color: #34495e; color: white; font-weight: bold;");
        appendLog("OS LOG: Mutex ENABLED. Race Conditions prevented.");
    } else {
        ui->btnToggleMutex->setText("🔴 Mutex OFF (RACE)");
        ui->btnToggleMutex->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold; border: 2px solid yellow;");
        appendLog("🚨 WARNING: Mutex DISABLED! Memory Overwrites / Race Conditions will occur.");
    }
}

void MainWindow::on_btnToggleSemaphore_clicked() {
    semaphoreEnabled = !semaphoreEnabled;
    if (semaphoreEnabled) {
        ui->btnToggleSemaphore->setText("🟢 Semaphore ON");
        ui->btnToggleSemaphore->setStyleSheet("background-color: #34495e; color: white; font-weight: bold;");
        appendLog("OS LOG: Semaphore ENABLED. Bounded capacity restored.");
    } else {
        ui->btnToggleSemaphore->setText("🔴 Sem OFF (OVERFLOW)");
        ui->btnToggleSemaphore->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold; border: 2px solid yellow;");
        appendLog("🚨 WARNING: Semaphore DISABLED! System capacity limits will be breached.");
    }
}

// Create two independent locks just for the deadlock demonstration
QMutex gateALock;
QMutex gateBLock;

void MainWindow::on_btnDeadlock_clicked() {
    appendLog("=========================================");
    appendLog("☠️ INITIATING DEADLOCK SCENARIO...");
    appendLog("Spawning two rogue threads requiring multiple locks...");
    
    // Thread 1: Grabs Gate A, then wants Gate B
    QThread* rogueThread1 = QThread::create([this]() {
        QMetaObject::invokeMethod(this, [this]() { appendLog("[Rogue 1] Locking Gate A..."); }, Qt::QueuedConnection);
        gateALock.lock();
        
        QThread::msleep(500); 
        
        QMetaObject::invokeMethod(this, [this]() { appendLog("[Rogue 1] Holding Gate A. Now trying to lock Gate B..."); }, Qt::QueuedConnection);
        gateBLock.lock(); // ⛔ IT WILL GET STUCK HERE FOREVER
        
        QMetaObject::invokeMethod(this, [this]() { appendLog("[Rogue 1] Success! (You will never see this message)"); }, Qt::QueuedConnection);
        gateBLock.unlock();
        gateALock.unlock();
    });

    // Thread 2: Grabs Gate B, then wants Gate A
    QThread* rogueThread2 = QThread::create([this]() {
        QMetaObject::invokeMethod(this, [this]() { appendLog("[Rogue 2] Locking Gate B..."); }, Qt::QueuedConnection);
        gateBLock.lock();
        
        QThread::msleep(500); 
        
        QMetaObject::invokeMethod(this, [this]() { appendLog("[Rogue 2] Holding Gate B. Now trying to lock Gate A..."); }, Qt::QueuedConnection);
        gateALock.lock(); // ⛔ IT WILL GET STUCK HERE FOREVER
        
        QMetaObject::invokeMethod(this, [this]() { appendLog("[Rogue 2] Success! (You will never see this message)"); }, Qt::QueuedConnection);
        gateALock.unlock();
        gateBLock.unlock();
    });

    // Start both threads simultaneously
    rogueThread1->start();
    rogueThread2->start();
    
    // Trigger the UI warning safely on the main thread
    QTimer::singleShot(1000, this, [this]() {
        appendLog("🚨 DEADLOCK DETECTED! Both threads are permanently frozen.");
        ui->lblState->setText("System State: DEADLOCKED ☠️");
        ui->lblState->setStyleSheet("color: red; font-weight: bold;");
    });
}
