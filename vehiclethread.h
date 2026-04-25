#ifndef VEHICLETHREAD_H
#define VEHICLETHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QString>

class VehicleThread : public QThread {
    Q_OBJECT
public:
    explicit VehicleThread(int id, int burstTime, int priority, QObject *parent = nullptr);
    int id;
    int burstTime;
    int priority;
    
    qint64 creationTime;
    qint64 startTime;
    qint64 finishTime;

    void wakeUp();

protected:
    void run() override;

signals:
    void logMessage(QString msg);
    void slotUpdated(int index, bool occupied, int vehicleId);
    void executionFinished(int id, int duration, qint64 waitTime, qint64 tat);
    void waitingForResource(int id);
    void resourceAcquired(int id);

private:
    bool isScheduled;
    QMutex waitMutex;
    QWaitCondition startCondition;
};
#endif
