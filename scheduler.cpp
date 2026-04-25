#include "scheduler.h"
#include <algorithm>
#include <QDateTime>

Scheduler::Scheduler(QObject *parent) : QObject(parent), currentAlgo(FCFS), totalCreated(0), totalCompleted(0), totalWaitTime(0), totalTurnaroundTime(0) {
    simStartTime = QDateTime::currentMSecsSinceEpoch();
    dispatchTimer = new QTimer(this);
    connect(dispatchTimer, &QTimer::timeout, this, &Scheduler::dispatch);
    dispatchTimer->start(200);

    autoGenTimer = new QTimer(this);
    connect(autoGenTimer, &QTimer::timeout, this, &Scheduler::autoGenerateTick);
}

void Scheduler::setAlgorithm(SchedulingAlgorithm algo) { currentAlgo = algo; }
void Scheduler::setAutoGenerate(bool enable) { enable ? autoGenTimer->start(1500) : autoGenTimer->stop(); }

void Scheduler::emitUpdates() {
    QStringList readyStrs;
    for(auto v : readyQueue) readyStrs << QString("V%1 (Prio:%2, Burst:%3)").arg(v->id).arg(v->priority).arg(v->burstTime);
    
    QString runningStr = "Running: ";
    for(auto v : runningList) runningStr += QString("V%1 ").arg(v->id);
    
    emit queuesUpdated(readyStrs, waitingListStr, runningStr);
    emit statsUpdated(readyQueue.size(), waitingListStr.size(), runningList.size());
    emit debugUpdated(parkingSlotsSemaphore.available(), isMutexLocked, waitingListStr.size());
}

void Scheduler::addVehicle(VehicleThread* vehicle) {
    totalCreated++;
    readyQueue.append(vehicle);
    connect(vehicle, &VehicleThread::executionFinished, this, &Scheduler::handleExecutionFinished);
    connect(vehicle, &VehicleThread::waitingForResource, this, &Scheduler::vehicleWaiting);
    connect(vehicle, &VehicleThread::resourceAcquired, this, &Scheduler::vehicleRunning);
    emitUpdates();
}

void Scheduler::dispatch() {
    if (currentState != RUNNING) return;
    
    // Check for Deadlock / System Overload
    if (parkingSlotsSemaphore.available() == 0 && waitingListStr.size() >= 4) {
        emit deadlockDetected();
    }

    if (readyQueue.isEmpty()) { emitUpdates(); return; }

    // FIXED LOGIC: Dispatch threads even if semaphore is 0, so they hit the block and populate the Waiting Queue!
    if (runningList.size() < 15) { // Max degree of multiprogramming
        if (currentAlgo == SJF) std::sort(readyQueue.begin(), readyQueue.end(), [](VehicleThread* a, VehicleThread* b) { return a->burstTime < b->burstTime; });
        else if (currentAlgo == PRIORITY) std::sort(readyQueue.begin(), readyQueue.end(), [](VehicleThread* a, VehicleThread* b) { return a->priority > b->priority; });

        VehicleThread* nextVehicle = readyQueue.takeFirst();
        runningList.append(nextVehicle);
        nextVehicle->wakeUp(); // Thread wakes up and will immediately hit the Semaphore block
    }
    emitUpdates();
}

void Scheduler::vehicleWaiting(int id) {
    waitingListStr.append(QString("V%1 [Blocked by Semaphore]").arg(id));
    emitUpdates();
}

void Scheduler::vehicleRunning(int id) {
    waitingListStr.removeAll(QString("V%1 [Blocked by Semaphore]").arg(id));
    emitUpdates();
}

void Scheduler::handleExecutionFinished(int id, int duration, qint64 wt, qint64 tat) {
    totalCompleted++;
    totalWaitTime += wt;
    totalTurnaroundTime += tat;
    
    for (int i = 0; i < runningList.size(); ++i) {
        if (runningList[i]->id == id) {
            VehicleThread* t = runningList.takeAt(i);
            t->deleteLater();
            break;
        }
    }
    
    double avgWT = totalCompleted > 0 ? (double)totalWaitTime / totalCompleted : 0.0;
    double avgTAT = totalCompleted > 0 ? (double)totalTurnaroundTime / totalCompleted : 0.0;
    qint64 elapsedSecs = (QDateTime::currentMSecsSinceEpoch() - simStartTime) / 1000;
    double tp = elapsedSecs > 0 ? (double)totalCompleted / elapsedSecs : 0.0;
    
    emit metricsUpdated(totalCreated, totalCompleted, avgWT, avgTAT, tp);
    emitUpdates();
}

void Scheduler::autoGenerateTick() {
    if(currentState == RUNNING) emit logMessage("Auto-Gen Triggered");
}
