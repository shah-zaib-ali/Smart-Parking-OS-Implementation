<div align="center">

<img src="https://img.shields.io/badge/OS-Ubuntu%2022.04-E95420?style=for-the-badge&logo=ubuntu&logoColor=white"/>
<img src="https://img.shields.io/badge/Framework-Qt%205.15-41CD52?style=for-the-badge&logo=qt&logoColor=white"/>
<img src="https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white"/>
<img src="https://img.shields.io/badge/Threading-POSIX%20pthreads-FF6B35?style=for-the-badge&logo=linux&logoColor=white"/>
<img src="https://img.shields.io/badge/License-Academic-8B5CF6?style=for-the-badge"/>

<br/><br/>

```
               ██████╗  █████╗ ██████╗ ██╗  ██╗██╗███╗   ██╗ ██████╗      ██████╗ ███████╗
               ██╔══██╗██╔══██╗██╔══██╗██║ ██╔╝██║████╗  ██║██╔════╝     ██╔═══██╗██╔════╝
               ██████╔╝███████║██████╔╝█████╔╝ ██║██╔██╗ ██║██║  ███╗    ██║   ██║███████╗
               ██╔═══╝ ██╔══██║██╔══██╗██╔═██╗ ██║██║╚██╗██║██║   ██║    ██║   ██║╚════██║
               ██║     ██║  ██║██║  ██║██║  ██╗██║██║ ╚████║╚██████╔╝    ╚██████╔╝███████║
```

# 🅿️ Ultimate Smart Parking OS Simulator

### *A real-time, multi-threaded visualization of Operating System internals*

**Making the invisible kernel visible — threads, mutexes, semaphores, scheduling & deadlocks, all live.**

<br/>

[![Made With Qt](https://img.shields.io/badge/Made%20with-Qt%20Creator-41CD52?style=flat-square&logo=qt)](https://www.qt.io/)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat-square)](https://isocpp.org/)
[![Threads](https://img.shields.io/badge/Multithreaded-Yes-success?style=flat-square)]()
[![Scheduling](https://img.shields.io/badge/Scheduling-FCFS%20%7C%20SJF%20%7C%20Priority-orange?style=flat-square)]()
[![Sync](https://img.shields.io/badge/Sync-Mutex%20%2B%20Semaphore-red?style=flat-square)]()
[![Deadlock](https://img.shields.io/badge/Deadlock-Simulated-critical?style=flat-square)]()

</div>

---

## 📋 Table of Contents

- [About The Project](#-about-the-project)
- [OS Concepts Demonstrated](#-os-concepts-demonstrated)
- [Screenshots](#-screenshots)
- [System Architecture](#-system-architecture)
- [Features](#-features)
- [Tech Stack](#-tech-stack)
- [Getting Started](#-getting-started)
- [How It Works](#-how-it-works)
- [Performance Metrics](#-performance-metrics)
- [Work Breakdown](#-work-breakdown)
- [Team](#-team)

---

## 🧠 About The Project

The **Ultimate Smart Parking OS Simulator** is a full-featured desktop application built in **C++ with Qt 5.15** that transforms abstract Operating Systems theory into a living, breathing visual demonstration.

> Every vehicle is a **thread**. Every parking slot is **shared memory**. Every arrival is a **system call**. Every conflict is a **race condition**.

The simulator models a parking lot where vehicles compete for limited slots — directly mirroring how an OS kernel manages concurrent processes racing for shared resources. All OS mechanics (scheduling, locking, blocking, deadlocking) happen in real-time and are rendered visually on screen.

```
┌─────────────────────────────────────────────────────────────────┐
│  VEHICLE ARRIVES  →  pthread_create()  →  sem_wait()            │
│       ↓                                        ↓                │
│  Ready Queue                           Blocked Queue            │
│       ↓                                        ↓                │
│  Scheduler (FCFS/SJF/Priority)    ←   sem_post() on departure   │
│       ↓                                                         │
│  pthread_mutex_lock()  →  CRITICAL SECTION  →  Slot Assigned    │
│       ↓                                                         │
│  usleep(burst_time)  →  pthread_mutex_unlock()  →  sem_post()   │
│       ↓                                                         │
│  pthread_join()  →  THREAD REAPED  →  Memory Freed              │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🎓 OS Concepts Demonstrated

<table>
<tr>
<td width="50%">

### 🔀 Multithreading & Process Management

| Phase | System Call | OS Analogy |
|-------|-------------|------------|
| **Birth** | `pthread_create()` | Process creation + TCB allocation |
| **Life** | `usleep()` | CPU burst → Waiting state |
| **Death** | `pthread_join()` | Thread reaping, memory reclaim |

Every vehicle is an independent `QThread` with its own **burst time** and **priority level**, mirroring CPU-bound processes managed by a kernel scheduler.

</td>
<td width="50%">

### 📅 CPU Scheduling Algorithms

| Algorithm | Strategy | Best For |
|-----------|----------|----------|
| ![FCFS](https://img.shields.io/badge/FCFS-First%20Come%20First%20Serve-blue?style=flat-square) | Strict arrival order | Fairness, simplicity |
| ![SJF](https://img.shields.io/badge/SJF-Shortest%20Job%20First-green?style=flat-square) | Min burst time first | Minimum avg wait time |
| ![Priority](https://img.shields.io/badge/Priority-VIP%20Preemption-red?style=flat-square) | Priority queue | Real-time critical tasks |

</td>
</tr>
<tr>
<td width="50%">

### 🔒 Synchronization — Mutex

**Problem:** Two threads scan the `ParkingLot[]` array simultaneously, both see `isOccupied = false`, and both claim Slot 1 → **Memory Corruption / Race Condition**.

**Solution:**
```cpp
pthread_mutex_lock(&parking_mutex);
    // ← ONLY ONE THREAD HERE AT A TIME →
    slot = findFreeSlot(ParkingLot);
    ParkingLot[slot].isOccupied = true;
    ParkingLot[slot].vehicleID = pthread_self();
pthread_mutex_unlock(&parking_mutex);
```

> 🛡️ **Result:** Zero race conditions across 101+ concurrent threads.

</td>
<td width="50%">

### 🚦 Resource Management — Semaphore

**Problem:** A Mutex only allows 1 thread, but the lot has **5 slots** — we need exactly N threads concurrently.

**Solution:**
```cpp
// Semaphore initialized to 5
sem_wait(&parking_sem);   // P-op: decrement; block if 0
    // ... occupy slot ...
sem_post(&parking_sem);   // V-op: increment; wake next thread
```

| Count | State |
|-------|-------|
| `5` | All slots free |
| `1–4` | Partially occupied |
| `0` | Full → next thread **BLOCKED** |

</td>
</tr>
<tr>
<td colspan="2">

### 💀 Deadlock Simulation — All 4 Coffman Conditions

> ⚠️ **Force Deadlock Mode** intentionally satisfies all four conditions, freezing two threads permanently.

| # | Coffman Condition | How It's Triggered |
|---|-------------------|-------------------|
| 1 | ![ME](https://img.shields.io/badge/1-Mutual%20Exclusion-red?style=flat-square) | Gate A and Gate B each have exclusive locks |
| 2 | ![HW](https://img.shields.io/badge/2-Hold%20%26%20Wait-orange?style=flat-square) | Thread 1 holds Gate A, waits for Gate B |
| 3 | ![NP](https://img.shields.io/badge/3-No%20Preemption-yellow?style=flat-square) | Neither thread releases its held lock |
| 4 | ![CW](https://img.shields.io/badge/4-Circular%20Wait-critical?style=flat-square) | T1 waits on T2; T2 waits on T1 → **FREEZE** |

```
Thread 1: HOLDS [Gate A] ──────────────── WAITING FOR [Gate B] ──┐
                                                                   │ DEADLOCK
Thread 2: HOLDS [Gate B] ──────────────── WAITING FOR [Gate A] ──┘
```

</td>
</tr>
</table>

---
## 📸 Screenshots

### Qt Creator — UI Design View
> The full four-panel dashboard layout visible in the Qt Creator designer.

![UI Design View](<Editing UI.png>)

---

### Simulation Running — All Slots Active
> 5 parking slots lit up green, OS Insight Panel showing live system state, Mutex ON, Semaphore ON.

![Running Simulation](<3.png>)

---

### Full Simulation Run — 101 Vehicles Processed
> Auto-generated run to completion. Statistics panel shows live metrics. Gantt chart at the bottom tracks every vehicle's execution timeline.

<img src="View Screen.png" alt="Full Simulation Run" width="100%"/>

---

## 🏗️ System Architecture

```
┌───────────────────────────────────────────────────────────────────────────┐
│                         USER SPACE (Qt UI Layer)                          │
│  ┌──────────────┐  ┌────────────────┐  ┌──────────────┐  ┌─────────────┐ │
│  │ Control Panel│  │  Parking Area  │  │  OS Insight  │  │  Statistics │ │
│  │  (Commands)  │  │(Critical Sect.)│  │    Panel     │  │  & Gantt    │ │
│  └──────┬───────┘  └───────┬────────┘  └──────┬───────┘  └──────┬──────┘ │
└─────────┼──────────────────┼─────────────────────────────────────┼────────┘
          │  Qt Signals/Slots │  (Thread-Safe Event Bridge)         │
┌─────────┼──────────────────┼─────────────────────────────────────┼────────┐
│         ▼   KERNEL LAYER   ▼    (OS Simulation Engine)            ▼        │
│  ┌─────────────────────────────────────────────────────────────────────┐  │
│  │                      Scheduler / Dispatcher                         │  │
│  │   ┌──────────────┐      ┌──────────────────┐    ┌───────────────┐  │  │
│  │   │  Ready Queue │ ───► │  CPU Scheduling   │───►│ Blocked Queue │  │  │
│  │   │  (Pending)   │      │ FCFS / SJF / Prio │    │  (Waiting)    │  │  │
│  │   └──────────────┘      └──────────────────┘    └───────────────┘  │  │
│  │                                                                      │  │
│  │   ┌─────────────┐      ┌─────────────────┐    ┌──────────────────┐ │  │
│  │   │  QSemaphore │      │  QMutex (Lock)   │    │  TCB Management  │ │  │
│  │   │  (Count: 5) │      │ Critical Section │    │ pthread_create() │ │  │
│  │   └─────────────┘      └─────────────────┘    │ pthread_join()   │ │  │
│  │                                                 └──────────────────┘ │  │
│  └─────────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────────┘
          │
┌─────────▼───────────────────────────────────────────────────────────────────┐
│                    SHARED MEMORY TABLE (Resource Layer)                      │
│                                                                              │
│   struct ParkingSlot {                                                       │
│       int       slotID;      // Logical Address → physical array index       │
│       bool      isOccupied;  // Dirty Bit → protected by Mutex               │
│       pthread_t vehicleID;   // Owner TCB handle → ownership validation      │
│       long      entryTime;   // Scheduling timestamp → WT / TAT metrics      │
│   };                                                                         │
│                                                                              │
│   ParkingSlot lot[5];  // Shared by ALL vehicle threads — DATA SEGMENT       │
└──────────────────────────────────────────────────────────────────────────────┘
```

---

## ✨ Features

| Feature | Description | Status |
|---------|-------------|--------|
| 🚗 **Vehicle Thread Spawning** | Each vehicle is a live `QThread` with burst time and priority | ✅ Live |
| 🤖 **Auto-Generate Mode** | Automatically spawns vehicles at intervals to stress-test | ✅ Live |
| 📅 **3 Scheduling Algorithms** | Switch between FCFS, SJF, and Priority mid-simulation | ✅ Live |
| 🔒 **Mutex Toggle** | Turn mutex protection ON/OFF to observe race conditions | ✅ Live |
| 🚦 **Semaphore Toggle** | Toggle counting semaphore to control concurrency | ✅ Live |
| 💀 **Force Deadlock** | Trigger intentional circular wait via two rogue threads | ✅ Live |
| 📊 **Real-Time Queue Graph** | Line chart mapping Ready, Blocked, Running over time | ✅ Live |
| 📈 **Live Metrics** | Avg WT, Avg TAT, Throughput updated every cycle | ✅ Live |
| 🗂️ **Gantt Chart** | Scrollable execution timeline per vehicle thread | ✅ Live |
| 📝 **Event Log** | Scrollable OS-level log of every thread event | ✅ Live |
| ⏯️ **Pause / Resume** | Freeze and resume the simulation at any point | ✅ Live |

---

## 🛠️ Tech Stack

<div align="center">

![C++](https://img.shields.io/badge/C%2B%2B17-Core%20Logic-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Qt](https://img.shields.io/badge/Qt%205.15-GUI%20%26%20Signals-41CD52?style=for-the-badge&logo=qt&logoColor=white)
![Ubuntu](https://img.shields.io/badge/Ubuntu%2022.04-Host%20OS-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)
![POSIX](https://img.shields.io/badge/POSIX-pthreads%20%2F%20sem-FF6B35?style=for-the-badge&logo=linux&logoColor=white)
![GCC](https://img.shields.io/badge/g%2B%2B-lpthread%20%2F%20lrt-A8B9CC?style=for-the-badge&logo=gnu&logoColor=black)

</div>

| Component | Technology | Purpose |
|-----------|-----------|---------|
| **Language** | C++17 | Core simulation engine |
| **GUI Framework** | Qt 5.15 | Widgets, charts, signal/slot mechanism |
| **Thread API** | QThread + POSIX pthreads | Concurrent vehicle process management |
| **Mutual Exclusion** | QMutex / pthread_mutex_t | Critical section protection |
| **Resource Counting** | QSemaphore / sem_t | Bounded concurrency (5 slots) |
| **Charting** | Qt Charts (QChartView) | Real-time queue density visualization |
| **OS** | Ubuntu 22.04 LTS | Native POSIX thread/semaphore support |
| **Compiler** | g++ | `-lpthread -lrt` flags |
| **IDE** | Qt Creator 5.15 | Build, design, debug environment |

---

## 🚀 Getting Started

### Prerequisites

```bash
# Install Qt5 development libraries
sudo apt update
sudo apt install qt5-default qtcharts5-dev build-essential

# Verify g++ is available
g++ --version
```

### Build & Run

```bash
# Clone the repository
git clone https://github.com/YOUR_USERNAME/SmartParkingOS.git
cd SmartParkingOS

# Open in Qt Creator (recommended)
qtcreator SmartParkingOS.pro

# Or build from terminal
qmake SmartParkingOS.pro
make -j$(nproc)

# Run the simulator
./SmartParkingOS
```

### Build Flags

```bash
# The .pro file must include:
QT       += core gui charts
LIBS     += -lpthread -lrt
CONFIG   += c++17
```

---

## ⚙️ How It Works

### Thread Lifecycle — Step by Step

```
 ARRIVE          WAIT              ENTER             PARK            DEPART
   │               │                 │                 │                │
   ▼               ▼                 ▼                 ▼                ▼
pthread_        sem_wait()      mutex_lock()       usleep()        mutex_unlock()
create()     ┌─────────────┐   ┌──────────┐    ┌──────────┐      sem_post()
   │         │ sem == 0?   │   │ Critical │    │  Burst   │      pthread_join()
   │    YES  │ → BLOCKED   │   │ Section  │    │  Time    │           │
   │◄────────│ queue       │   │ (Atomic) │    │(usleep)  │           │
   │    NO   │ → CONTINUE  │   └──────────┘    └──────────┘           ▼
   │         └─────────────┘                                      REAPED ✓
   ▼
Ready Queue
(Scheduler picks next)
```

### Scheduling Decision Flow

```
                    ┌─────────────────────────────┐
                    │      SLOT FREED (sem_post)   │
                    └──────────────┬──────────────┘
                                   ▼
                    ┌─────────────────────────────┐
                    │   What algorithm is active?  │
                    └──────────────┬──────────────┘
              ┌────────────────────┼────────────────────┐
              ▼                    ▼                    ▼
    ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
    │      FCFS       │  │      SJF        │  │    PRIORITY     │
    │  Pick OLDEST    │  │  Pick SHORTEST  │  │  Pick HIGHEST   │
    │  arrival time   │  │  burst time     │  │  priority value │
    └────────┬────────┘  └────────┬────────┘  └────────┬────────┘
             └───────────────────►▼◄───────────────────┘
                         ┌─────────────┐
                         │  DISPATCH   │
                         │  VEHICLE    │
                         └─────────────┘
```

---

## 📊 Performance Metrics

Results from a full auto-generate run (**101 vehicles**, FCFS, Ubuntu 22.04):

| Metric | Value | Notes |
|--------|-------|-------|
| **Vehicles Created** | 101 | All threads successfully spawned |
| **Vehicles Completed** | 101 | 100% completion, zero zombie threads |
| **Average Waiting Time** | `603.5 ms` | Time in Ready/Blocked queues |
| **Average Turnaround Time** | `2448.8 ms` | Creation → pthread_join() |
| **Throughput** | `1.49 vehicles/sec` | With 5 concurrent slots |
| **Race Conditions** | `0` | Mutex fully protected shared memory |
| **Thread Leaks** | `0` | All threads reaped via pthread_join() |

### Scheduling Algorithm Comparison

```
Avg Waiting Time (lower = better)
─────────────────────────────────────────────────────────────
  SJF      ████████░░░░░░░░░░░░░░░░░░░░  MINIMUM (Optimal)
  Priority ████████████░░░░░░░░░░░░░░░░  Variable
  FCFS     ████████████████████░░░░░░░░  Highest (Convoy Effect)
─────────────────────────────────────────────────────────────
```

---

## 📁 Project Structure

```
SmartParkingOS/
│
├── 📄 SmartParkingOS.pro       # Qt project file (build config, flags)
│
├── 📁 src/
│   ├── main.cpp                # Entry point
│   ├── mainwindow.cpp          # UI logic, signal/slot connections
│   ├── mainwindow.h
│   ├── mainwindow.ui           # Qt Designer layout file
│   │
│   ├── vehicle.cpp             # QThread subclass — vehicle lifecycle
│   ├── vehicle.h               # Thread Control Block (TCB) struct
│   │
│   ├── scheduler.cpp           # FCFS / SJF / Priority dispatcher
│   ├── scheduler.h             # Ready Queue + Blocked Queue management
│   │
│   ├── parkinglot.cpp          # Shared memory table (ParkingSlot[5])
│   ├── parkinglot.h            # Mutex + Semaphore synchronization
│   │
│   └── deadlock.cpp            # Coffman condition demo (rogue threads)
│
└── 📁 docs/
    └── Smart_Parking_OS_Report.docx
```

---

## 📌 Work Breakdown

| Milestone | Description | Owner | Sprint |
|-----------|-------------|-------|--------|
| **M1 — Architecture** | Shared memory table design, ParkingSlot struct, command parsing | Ali Raza | Week 1 |
| **M2 — Sync Core** | Semaphore + Mutex wrappers, thread lifecycle (create/join) | Shahzaib Ali | Week 2 |
| **M3 — GUI Integration** | Qt 5 dashboard, Gantt chart, real-time queue graph | Faris Altaf | Week 2 |
| **M4 — Debugging** | Race condition stress tests, deadlock verification | Group | Week 3 |
| **M5 — Final Prep** | Documentation, report, performance benchmarking | Group | Week 4 |

---

## 👥 Team

<div align="center">

| | Name | Student ID | Contribution |
|--|------|-----------|--------------|
| 👤 | **Shahzaib Ali** | 24K-0529 | Synchronization Core — Semaphores, Mutex, Thread Lifecycle |
| 👤 | **Faris Altaf** | 24K-0879 | GUI Integration — Qt Dashboard, Gantt Chart, Real-time Graph |
| 👤 | **Ali Raza** | 24K-0747 | System Architecture — Shared Memory Table, Command Parsing |

**Course:** CS-2006 Operating Systems · **Section:** BCS 4B · **Bahria University, Karachi**

</div>

---

## 📚 References

- Silberschatz, Galvin & Gagne — *Operating System Concepts*, 10th Ed.
- Tanenbaum & Bos — *Modern Operating Systems*, 4th Ed.
- [Qt Documentation — QThread](https://doc.qt.io/qt-5/qthread.html)
- [Qt Documentation — QMutex](https://doc.qt.io/qt-5/qmutex.html)
- [POSIX Threads Programming — LLNL](https://hpc-tutorials.llnl.gov/posix/)

---

<div align="center">

**CS-2006 Operating Systems · Bahria University · BCS 4B · 2026**

*"Making the invisible kernel visible — one thread at a time."*

</div>
