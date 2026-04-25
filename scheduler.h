#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QTimer>
#include "vehiclethread.h"
#include "globals.h"

class Scheduler : public QObject {
    Q_OBJECT
public:
    explicit Scheduler(QObject *parent = nullptr);
    void addVehicle(VehicleThread* vehicle);
    void setAlgorithm(SchedulingAlgorithm algo);
    void setAutoGenerate(bool enable);

signals:
    void logMessage(QString msg);
    void queuesUpdated(QStringList readyList, QStringList waitingList, QString runningStr);
    void statsUpdated(int readyCount, int waitCount, int runningCount);
    void metricsUpdated(int total, int completed, double avgWT, double avgTAT, double throughput);
    void debugUpdated(int semValue, bool mutexLocked, int blockedCount);
    void deadlockDetected(); // <-- NEW DEADLOCK SIGNAL

public slots:
    void dispatch();
    void handleExecutionFinished(int id, int duration, qint64 wt, qint64 tat);
    void vehicleWaiting(int id);
    void vehicleRunning(int id);
    void autoGenerateTick();

private:
    void emitUpdates();
    QList<VehicleThread*> readyQueue;
    QList<VehicleThread*> runningList;
    QStringList waitingListStr;
    SchedulingAlgorithm currentAlgo;
    QTimer* dispatchTimer;
    QTimer* autoGenTimer;
    
    int totalCreated;
    int totalCompleted;
    qint64 totalWaitTime;
    qint64 totalTurnaroundTime;
    qint64 simStartTime;
};
#endif
