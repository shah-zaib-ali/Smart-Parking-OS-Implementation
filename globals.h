#ifndef GLOBALS_H
#define GLOBALS_H

#include <QMutex>
#include <QSemaphore>
#include <QWaitCondition>

const int TOTAL_SLOTS = 5;

extern QSemaphore parkingSlotsSemaphore;
extern QMutex criticalSectionMutex;
extern bool slotStatus[5];
extern bool isMutexLocked;
extern bool mutexEnabled;     // <-- NEW
extern bool semaphoreEnabled; // <-- NEW

enum SchedulingAlgorithm { FCFS, SJF, PRIORITY };
enum SystemState { IDLE, RUNNING, PAUSED };

extern SystemState currentState;
extern int simulationSpeed;

#endif
