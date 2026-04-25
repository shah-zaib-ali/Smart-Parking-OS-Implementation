#include "vehiclethread.h"
#include "globals.h"
#include <QDateTime>
#include <QRandomGenerator>

VehicleThread::VehicleThread(int id, int burstTime, int priority, QObject *parent)
    : QThread(parent), id(id), burstTime(burstTime), priority(priority), isScheduled(false) {
    creationTime = QDateTime::currentMSecsSinceEpoch();
}

void VehicleThread::wakeUp() {
    QMutexLocker locker(&waitMutex);
    isScheduled = true;
    startCondition.wakeOne();
}

void VehicleThread::run() {
    emit logMessage(QString("[%1] V%2 Arrived. (Burst: %3ms, Prio: %4)").arg(QDateTime::currentDateTime().toString("HH:mm:ss")).arg(id).arg(burstTime).arg(priority));

    waitMutex.lock();
    while (!isScheduled || currentState == PAUSED) startCondition.wait(&waitMutex);
    waitMutex.unlock();

    startTime = QDateTime::currentMSecsSinceEpoch();

    // ---------------- 1. SEMAPHORE LOGIC (CAPACITY) ----------------
    if (semaphoreEnabled) {
        emit waitingForResource(id);
        parkingSlotsSemaphore.acquire();
        emit resourceAcquired(id);
    } else {
        emit logMessage(QString("🚧 V%1 ignored Semaphore! (No capacity check)").arg(id));
    }

    // ---------------- 2. MUTEX LOGIC (CRITICAL SECTION) ----------------
    if (mutexEnabled) {
        criticalSectionMutex.lock();
        isMutexLocked = true;
    }

    int assignedSlot = -1;
    for (int i = 0; i < TOTAL_SLOTS; ++i) {
        if (!slotStatus[i]) {
            if (!mutexEnabled) msleep(50); // Artificially delay to force a Race Condition collision
            slotStatus[i] = true;
            assignedSlot = i;
            break;
        }
    }

    if (mutexEnabled) {
        isMutexLocked = false;
        criticalSectionMutex.unlock();
    }

    // ---------------- 3. CHAOS OUTCOMES ----------------
    if (!semaphoreEnabled && assignedSlot == -1) {
        // Lot was actually full, but Semaphore didn't stop them! OVERFLOW!
        assignedSlot = QRandomGenerator::global()->bounded(0, TOTAL_SLOTS);
        emit logMessage(QString("💥 OVERFLOW BREACH: V%1 forced into occupied Slot %2!").arg(id).arg(assignedSlot+1));
    } else if (!mutexEnabled && assignedSlot != -1) {
        // They found a slot, but did they overwrite someone else?
        emit logMessage(QString("⚠️ RACE CONDITION: V%1 blindly grabbed Slot %2!").arg(id).arg(assignedSlot+1));
    }

    // ---------------- 4. EXECUTION ----------------
    if (assignedSlot != -1) {
        emit slotUpdated(assignedSlot, true, id);
        if (mutexEnabled && semaphoreEnabled) {
            emit logMessage(QString("[%1] V%2 executing safely in Slot %3.").arg(QDateTime::currentDateTime().toString("HH:mm:ss")).arg(id).arg(assignedSlot+1));
        }
        
        msleep(burstTime / simulationSpeed); 

        // ---------------- 5. CLEANUP ----------------
        if (mutexEnabled) criticalSectionMutex.lock();
        isMutexLocked = true;
        slotStatus[assignedSlot] = false;
        isMutexLocked = false;
        if (mutexEnabled) criticalSectionMutex.unlock();

        emit slotUpdated(assignedSlot, false, -1);
    }
    
    if (semaphoreEnabled) parkingSlotsSemaphore.release();
    
    finishTime = QDateTime::currentMSecsSinceEpoch();
    emit executionFinished(id, burstTime, startTime - creationTime, finishTime - creationTime);
}
