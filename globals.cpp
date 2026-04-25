#include "globals.h"

QSemaphore parkingSlotsSemaphore(TOTAL_SLOTS);
QMutex criticalSectionMutex;
bool slotStatus[5] = {false, false, false, false, false};
bool isMutexLocked = false;
bool mutexEnabled = true;     // Default ON
bool semaphoreEnabled = true; // Default ON

SystemState currentState = IDLE;
int simulationSpeed = 1;
