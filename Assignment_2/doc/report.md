# Smart Drone Hangar - Assignment 02

**Embedded Systems and IoT - ISI LT - a.y. 2025/2026**

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [System Overview](#2-system-overview)
3. [Hardware Architecture](#3-hardware-architecture)
4. [Software Architecture](#4-software-architecture)
5. [Finite State Machines](#5-finite-state-machines)
6. [Serial Communication Protocol](#6-serial-communication-protocol)
7. [Configuration Parameters](#7-configuration-parameters)
8. [Implementation Details](#8-implementation-details)
9. [Breadboard Schema](#9-breadboard-schema)
10. [Demo Video](#10-demo-video)

---

## 1. Introduction

This report describes the design and implementation of the **Smart Drone Hangar** system, an embedded prototype that manages a drone hangar with automated door control, presence detection, distance monitoring, and temperature-based safety features.

The system consists of two cooperating subsystems:
- **Drone Hangar (Arduino UNO)**: Controls physical hardware including sensors, actuators, and user interface
- **Drone Remote Unit (PC/JavaFX)**: Provides a GUI to simulate drone commands and visualize system state

---

## 2. System Overview

### 2.1 Functional Requirements

The Smart Drone Hangar implements the following functionality:

| Phase | Description |
|-------|-------------|
| **Startup** | Door closed, drone assumed inside, L1 on, LCD shows "DRONE INSIDE" |
| **Take-off** | Door opens on command, waits for drone to exit (distance > D1 for T1), then closes |
| **Operating** | Drone is outside, system awaits landing command |
| **Landing** | PIR detects drone, door opens, monitors distance until drone lands (distance < D2 for T2) |
| **Temperature Monitoring** | Continuous monitoring with pre-alarm and alarm states |

### 2.2 System Block Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           DRONE HANGAR (Arduino UNO)                        │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐        │
│  │  PIR Sensor │  │  Ultrasonic │  │ Temp Sensor │  │   Button    │        │
│  │    (DPD)    │  │    (DDD)    │  │   (LM35)    │  │   (RESET)   │        │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘        │
│         │                │                │                │               │
│         └────────────────┴────────────────┴────────────────┘               │
│                                   │                                         │
│                          ┌────────▼────────┐                               │
│                          │   SCHEDULER     │                               │
│                          │  (TimerOne)     │                               │
│                          │   100ms base    │                               │
│                          └────────┬────────┘                               │
│                                   │                                         │
│         ┌─────────────────────────┼─────────────────────────┐              │
│         │                         │                         │              │
│  ┌──────▼──────┐  ┌───────────────▼───────────────┐  ┌──────▼──────┐      │
│  │  TempTask   │  │      DroneHangarTask          │  │  BlinkTask  │      │
│  │  (5 states) │  │        (7 states)             │  │  (L1,L2,L3) │      │
│  └──────┬──────┘  └───────────────┬───────────────┘  └──────┬──────┘      │
│         │                         │                         │              │
│         └─────────────────────────┴─────────────────────────┘              │
│                                   │                                         │
│  ┌─────────────┐  ┌─────────────┐ │ ┌─────────────┐  ┌─────────────┐       │
│  │  Servo (HD) │  │  LCD (I2C)  │ │ │  LED L1     │  │  LED L2/L3  │       │
│  └─────────────┘  └─────────────┘ │ └─────────────┘  └─────────────┘       │
│                                   │                                         │
└───────────────────────────────────┼─────────────────────────────────────────┘
                                    │ Serial (9600 baud)
                                    │
┌───────────────────────────────────▼─────────────────────────────────────────┐
│                        DRONE REMOTE UNIT (PC/JavaFX)                        │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐             │
│  │  TAKE OFF btn   │  │    LAND btn     │  │  State Display  │             │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 3. Hardware Architecture

### 3.1 Components

| Component | Type | Pin | Function |
|-----------|------|-----|----------|
| **L1** | Green LED | D13 | Drone inside indicator |
| **L2** | Green LED | D12 | Take-off/landing in progress (blinking) |
| **L3** | Red LED | D11 | Alarm indicator |
| **HD** | Servo Motor | D9 | Hangar door control (0°=closed, 90°=open) |
| **DDD** | HC-SR04 Ultrasonic | D4 (Trig), D5 (Echo) | Drone distance measurement |
| **DPD** | PIR Sensor | D3 | Motion detection for landing approach |
| **RESET** | Tactile Button | D2 | Alarm reset |
| **TEMP** | LM35 | A1 | Temperature monitoring |
| **LCD** | I2C LCD 16x2 | A4 (SDA), A5 (SCL) | Operator display |

### 3.2 Pin Configuration

```
Arduino UNO Pin Mapping
═══════════════════════════════════════════════════════════
    ┌─────────────────────────────────────┐
    │  [USB]                    [POWER]   │
    │                                     │
    │  D13 ────────── L1 (Green LED)     │
    │  D12 ────────── L2 (Green LED)     │
    │  D11 ────────── L3 (Red LED)       │
    │  D9  ────────── Servo (HD)         │
    │  D5  ────────── Ultrasonic Echo    │
    │  D4  ────────── Ultrasonic Trig    │
    │  D3  ────────── PIR Sensor         │
    │  D2  ────────── Reset Button       │
    │                                     │
    │  A1  ────────── LM35 Temp Sensor   │
    │  A4  ────────── LCD SDA (I2C)      │
    │  A5  ────────── LCD SCL (I2C)      │
    │                                     │
    └─────────────────────────────────────┘
```

---

## 4. Software Architecture

### 4.1 Task-Based Cooperative Scheduling

The system implements a **task-based architecture** using cooperative multitasking with the TimerOne hardware timer. This approach ensures deterministic timing and clean separation of concerns.

```
┌─────────────────────────────────────────────────────────────┐
│                    SCHEDULER ARCHITECTURE                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   TimerOne Hardware Interrupt (100ms)                       │
│              │                                              │
│              ▼                                              │
│   ┌─────────────────────┐                                  │
│   │  timerFlag = true   │                                  │
│   └──────────┬──────────┘                                  │
│              │                                              │
│              ▼                                              │
│   ┌─────────────────────────────────────────┐              │
│   │  Scheduler::schedule()                   │              │
│   │  ├── Wait for timerFlag                  │              │
│   │  ├── For each task:                      │              │
│   │  │   ├── Check if active                 │              │
│   │  │   ├── updateAndCheckTime(basePeriod)  │              │
│   │  │   └── If period elapsed: tick()       │              │
│   │  └── Clear timerFlag                     │              │
│   └─────────────────────────────────────────┘              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 Task Configuration

| Task | Period (ms) | Priority | Description |
|------|-------------|----------|-------------|
| `DroneHangarTask` | 100 | High | Main control FSM |
| `ButtonTask` | 50 | High | Debounced button input |
| `ServoTask` | 100 | Medium | Door position control |
| `PIRTask` | 100 | Medium | Motion detection polling |
| `LCDTask` | 200 | Low | Display updates |
| `UltrasonicTask` | 1000 | Low | Distance measurement |
| `TempTask` | 1000 | Medium | Temperature monitoring |
| `BlinkTask` (×3) | 500 | Low | LED blinking control |

### 4.3 Project Structure

```
Assignment_2/
├── platformio.ini                 # Build configuration
├── src/drone-hangar/
│   ├── main.cpp                   # Entry point, main loop
│   ├── config.h                   # Parameters and pin definitions
│   ├── core.cpp/h                 # System initialization
│   ├── Kernel/
│   │   ├── Scheduler.cpp/h        # Task scheduler with TimerOne
│   │   └── kernel.cpp/h           # State management utilities
│   ├── Devices/                   # Hardware Abstraction Layer
│   │   ├── Button.cpp/h           # Debounced button input
│   │   ├── Led.cpp/h              # LED control
│   │   ├── LCD.cpp/h              # I2C LCD wrapper
│   │   ├── ServoMotor.cpp/h       # Servo control
│   │   ├── PIRSensor.cpp/h        # PIR sensor wrapper
│   │   ├── UltrasonicSensor.cpp/h # HC-SR04 driver
│   │   └── TempSensorLM35.cpp/h   # LM35 temperature sensor
│   └── Tasks/                     # Task implementations
│       ├── Task.h                 # Abstract base class
│       ├── DroneHangarTask.cpp/h  # Main FSM (7 states)
│       ├── TempTask.cpp/h         # Temperature FSM (5 states)
│       ├── BlinkTask.cpp/h        # LED blinking
│       ├── ButtonTask.cpp/h       # Button polling
│       ├── ServoTask.cpp/h        # Servo control
│       ├── LCDTask.cpp/h          # LCD updates
│       ├── PIRTask.cpp/h          # PIR polling
│       └── UltrasonicTask.cpp/h   # Distance measurement
└── drone-remote-unit/             # JavaFX PC application
    ├── pom.xml                    # Maven build
    └── src/main/java/dru/
        ├── DroneRemoteUnit.java   # Application entry
        ├── model/                 # State enums and model
        ├── serial/                # Serial communication
        └── ui/                    # JavaFX controller
```

---

## 5. Finite State Machines

### 5.1 DroneHangarTask FSM (Main Control)

The main control logic is implemented as a 7-state FSM that manages the drone hangar operations.

```
                              ┌─────────────────────────────────────────────────────────────┐
                              │              DRONE HANGAR TASK FSM                          │
                              └─────────────────────────────────────────────────────────────┘

                                              TAKEOFF cmd
                                           (not in pre-alarm)
                    ┌──────────────────────────────────────────────────────┐
                    │                                                      │
                    │                                                      ▼
            ┌───────┴───────┐                                     ┌───────────────┐
            │               │                                     │               │
      ┌────►│ DRONE_INSIDE  │                                     │   TAKE_OFF    │
      │     │               │                                     │               │
      │     │  • L1 ON      │                                     │  • Door OPEN  │
      │     │  • L2 OFF     │                                     │  • L2 BLINK   │
      │     │  • Door CLOSED│                                     │  • LCD: TAKE  │
      │     │  • LCD: DRONE │                                     │    OFF        │
      │     │    INSIDE     │                                     │               │
      │     └───────────────┘                                     └───────┬───────┘
      │                                                                   │
      │                                                                   │ distance > D1
      │                                                                   ▼
      │     ┌───────────────┐          T1 elapsed              ┌───────────────────┐
      │     │               │◄─────────────────────────────────│                   │
      │     │   DRONE_OUT   │      (distance > D1 for T1)      │  TAKE_OFF_CHECK   │
      │     │               │                                  │                   │
      │     │  • L2 OFF     │        distance < D1             │  • Verify exit    │
      │     │  • Door CLOSED│   ┌──────────────────────────────│  • Timer check    │
      │     │  • LCD: DRONE │   │  (drone didn't leave)        │                   │
      │     │    OUT        │   │                              └───────────────────┘
      │     └───────┬───────┘   │                                        │
      │             │           │                                        │
      │   LAND cmd  │           │           distance < D1                │
      │(not in pre- │           └────────────────────────────────────────┼──────────┐
      │   alarm)    │                                                    │          │
      │             ▼                                                    │          │
      │     ┌───────────────┐                                           │          │
      │     │               │                                           │          │
      │     │    DETECT     │◄──────────────────────────────────────────┘          │
      │     │               │                                                      │
      │     │  • Wait for   │                                                      │
      │     │    PIR motion │                                                      │
      │     │               │                                                      │
      │     └───────┬───────┘                                                      │
      │             │                                                              │
      │             │ PIR detects motion                                           │
      │             ▼                                                              │
      │     ┌───────────────┐                                                      │
      │     │               │          distance > D2                               │
      │     │    LANDING    │◄─────────────────────────┐                          │
      │     │               │      (drone bounced)     │                          │
      │     │  • Door OPEN  │                          │                          │
      │     │  • L2 BLINK   │                          │                          │
      │     │  • Send DIST  │                          │                          │
      │     │               │                          │                          │
      │     └───────┬───────┘                          │                          │
      │             │                                  │                          │
      │             │ distance < D2                    │                          │
      │             ▼                                  │                          │
      │     ┌───────────────────┐                      │                          │
      │     │                   │──────────────────────┘                          │
      │     │   LANDING_CHECK   │                                                 │
      │     │                   │                                                 │
      │     │  • Verify landing │                                                 │
      │     │  • Timer check    │                                                 │
      │     │  • Send DIST      │                                                 │
      │     └─────────┬─────────┘                                                 │
      │               │                                                           │
      │               │ T2 elapsed (distance < D2 for T2)                         │
      │               │                                                           │
      └───────────────┴───────────────────────────────────────────────────────────┘
```

#### State Descriptions

| State | Entry Actions | Active Conditions | Exit Transitions |
|-------|---------------|-------------------|------------------|
| `DRONE_INSIDE` | L1 ON, L2 OFF, Door CLOSED, LCD "DRONE INSIDE" | Awaiting TAKEOFF command | TAKEOFF cmd → `TAKE_OFF` |
| `TAKE_OFF` | L2 BLINK, Door OPEN, LCD "TAKE OFF" | Monitoring distance | dist > D1 → `TAKE_OFF_CHECK` |
| `TAKE_OFF_CHECK` | Start timer | Verify drone exit | T1 elapsed → `DRONE_OUT`; dist < D1 → `DRONE_INSIDE` |
| `DRONE_OUT` | L2 OFF, Door CLOSED, LCD "DRONE OUT" | Awaiting LAND command | LAND cmd → `DETECT` |
| `DETECT` | - | Awaiting PIR detection | PIR motion → `LANDING` |
| `LANDING` | L2 BLINK, Door OPEN, Send DIST | Monitoring descent | dist < D2 → `LANDING_CHECK` |
| `LANDING_CHECK` | Start timer | Verify landing | T2 elapsed → `DRONE_INSIDE`; dist > D2 → `LANDING` |

---

### 5.2 TempTask FSM (Temperature Monitoring)

The temperature monitoring system runs in parallel and can override main operations when unsafe conditions are detected.

```
                         ┌─────────────────────────────────────────────────────────┐
                         │                 TEMPERATURE TASK FSM                     │
                         └─────────────────────────────────────────────────────────┘

                                     temp ≥ Temp1
        ┌────────────────────────────────────────────────────────┐
        │                                                        │
        │                                                        ▼
┌───────┴───────┐      temp < Temp1                      ┌───────────────┐
│               │◄───────────────────────────────────────│               │
│    NORMAL     │                                        │   HIGHTEMP    │
│               │      temp < Temp1                      │               │
│  • Normal ops │◄──────────────────────┐                │  • Start T3   │
│  • All cmds   │                       │                │    timer      │
│    allowed    │                       │                │               │
└───────────────┘                       │                └───────┬───────┘
        ▲                               │                        │
        │                               │                        │ T3 elapsed
        │                               │                        │ (temp ≥ Temp1 for T3)
        │                               │                        ▼
        │                               │                ┌───────────────┐
        │                               │                │               │
        │                               └────────────────│   PREALARM    │
        │                                                │               │
        │                                                │  • Block new  │
        │                                                │    operations │
        │                                                │  • Allow      │
        │                                                │    in-progress│
        │                                                └───────┬───────┘
        │                                                        │
        │                                                        │ temp ≥ Temp2
        │            temp < Temp2                                ▼
        │         ┌──────────────────────────────────────┌───────────────┐
        │         │                                      │               │
        │         │                                      │  VERYHIGHTEMP │
        │         │                                      │               │
        │         │                                      │  • Start T4   │
        │         │                                      │    timer      │
        │         │                                      │               │
        │         │                                      └───────┬───────┘
        │         │                                              │
        │         │                                              │ T4 elapsed
        │         │                                              │ (temp ≥ Temp2 for T4)
        │         ▼                                              ▼
        │  ┌───────────────┐                             ┌───────────────┐
        │  │   PREALARM    │                             │               │
        │  └───────────────┘                             │     ALARM     │
        │                                                │               │
        │                                                │  • Door CLOSE │
        │                                                │  • L3 ON      │
        │                                                │  • LCD: ALARM │
        │                                                │  • Send ALARM │
        │                                                │    to PC      │
        │                                                │  • All ops    │
        │                                                │    suspended  │
        │                                                └───────┬───────┘
        │                                                        │
        │                          RESET button pressed          │
        └────────────────────────────────────────────────────────┘
```

#### State Descriptions

| State | Temperature Condition | System Impact | Exit Transitions |
|-------|----------------------|---------------|------------------|
| `NORMAL` | T < Temp1 | All operations allowed | temp ≥ Temp1 → `HIGHTEMP` |
| `HIGHTEMP` | T ≥ Temp1 | Timer started, monitoring | T < Temp1 → `NORMAL`; T3 elapsed → `PREALARM` |
| `PREALARM` | T ≥ Temp1 for T3 | New take-offs/landings blocked | T < Temp1 → `NORMAL`; T ≥ Temp2 → `VERYHIGHTEMP` |
| `VERYHIGHTEMP` | T ≥ Temp2 | Timer started, critical | T < Temp2 → `PREALARM`; T4 elapsed → `ALARM` |
| `ALARM` | T ≥ Temp2 for T4 | All ops suspended, door closed, L3 ON | RESET button → `NORMAL` |

---

### 5.3 FSM Interaction

The two FSMs interact through the following mechanism:

```
┌─────────────────────────────────────────────────────────────────┐
│                     FSM INTERACTION                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   DroneHangarTask                    TempTask                   │
│        │                                 │                      │
│        │    ┌───────────────────────┐    │                      │
│        │    │  isInPreAlarm()?      │◄───┤                      │
│        │    │  isInAlarm()?         │    │                      │
│        │    │  isHighTemp()?        │    │                      │
│        │    └───────────────────────┘    │                      │
│        │              │                  │                      │
│        │              ▼                  │                      │
│        │    ┌───────────────────────┐    │                      │
│        │    │ Pre-alarm: Block new  │    │                      │
│        │    │ TAKEOFF/LAND commands │    │                      │
│        │    └───────────────────────┘    │                      │
│        │              │                  │                      │
│        │              ▼                  │                      │
│        │    ┌───────────────────────┐    │                      │
│        │    │ Alarm: Suspend all    │    │                      │
│        │    │ operations, close door│    │                      │
│        │    └───────────────────────┘    │                      │
│        │                                 │                      │
└─────────────────────────────────────────────────────────────────┘
```

---

## 6. Serial Communication Protocol

### 6.1 Protocol Overview

Communication uses **9600 baud** serial with newline-terminated ASCII messages.

### 6.2 Commands (PC → Arduino)

| Command | Description | Valid States |
|---------|-------------|--------------|
| `TAKEOFF` | Request hangar door open for take-off | `DRONE_INSIDE` (not in pre-alarm) |
| `LAND` | Request hangar door open for landing | `DRONE_OUT` (not in pre-alarm) |
| `STATUS?` | Query current system state | Any |

### 6.3 Responses (Arduino → PC)

| Response | Description |
|----------|-------------|
| `INSIDE` | Drone is inside hangar at rest |
| `OUTSIDE` | Drone is outside operating |
| `TAKEOFF` | Drone is taking off |
| `LANDING` | Drone is landing |
| `NORMAL` | Temperature normal |
| `PREALARM` | Temperature pre-alarm active |
| `ALARM` | System in alarm state |
| `DIST:<value>` | Current distance in cm (during landing) |

### 6.4 Communication Sequence Diagrams

#### Take-off Sequence

```
    PC (DRU)                          Arduino (Hangar)
       │                                    │
       │  ───── "TAKEOFF\n" ─────────────►  │
       │                                    │  [Open door, start monitoring]
       │  ◄───── "TAKEOFF\n" ─────────────  │
       │                                    │
       │           ... time passes ...      │
       │                                    │  [Distance > D1 for T1]
       │  ◄───── "OUTSIDE\n" ────────────   │
       │                                    │  [Close door]
       │                                    │
```

#### Landing Sequence

```
    PC (DRU)                          Arduino (Hangar)
       │                                    │
       │  ───── "LAND\n" ────────────────►  │
       │                                    │  [Wait for PIR]
       │                                    │  [PIR detected, open door]
       │  ◄───── "LANDING\n" ────────────   │
       │                                    │
       │  ◄───── "DIST:45\n" ────────────   │
       │  ◄───── "DIST:38\n" ────────────   │
       │  ◄───── "DIST:25\n" ────────────   │  [Distance < D2]
       │  ◄───── "DIST:22\n" ────────────   │
       │           ... T2 elapsed ...       │
       │  ◄───── "INSIDE\n" ─────────────   │
       │                                    │  [Close door]
       │                                    │
```

---

## 7. Configuration Parameters

All configurable parameters are defined in `config.h`:

### 7.1 Distance Thresholds

| Parameter | Value | Unit | Description |
|-----------|-------|------|-------------|
| `D1` | 50 | cm | Minimum distance to confirm drone exit |
| `D2` | 30 | cm | Maximum distance to confirm drone landing |

### 7.2 Time Thresholds

| Parameter | Value | Unit | Description |
|-----------|-------|------|-------------|
| `T1` | 3000 | ms | Confirmation time for take-off (distance > D1) |
| `T2` | 3000 | ms | Confirmation time for landing (distance < D2) |
| `T3` | 3000 | ms | Time before pre-alarm (temp ≥ Temp1) |
| `T4` | 3000 | ms | Time before alarm (temp ≥ Temp2) |

### 7.3 Temperature Thresholds

| Parameter | Value | Unit | Description |
|-----------|-------|------|-------------|
| `Temp1` | 20.0 | °C | Pre-alarm temperature threshold |
| `Temp2` | 25.0 | °C | Alarm temperature threshold |

### 7.4 Rationale

The parameter values were chosen for practical prototyping and testing:

- **D1 = 50cm**: Sufficient to distinguish between drone inside (close) and outside (far)
- **D2 = 30cm**: Indicates drone has descended close to landing pad
- **T1/T2 = 3s**: Long enough to filter noise but responsive
- **Temp1/Temp2**: Low values for easy testing (can be raised for production)

---

## 8. Implementation Details

### 8.1 Task Base Class

```cpp
class Task {
  int myPeriod;      // Execution period in ms
  int timeElapsed;   // Time since last execution
  bool active;       // Task enabled flag

public:
  virtual void init(int period);
  virtual void tick() = 0;  // Pure virtual - implement FSM logic
  bool updateAndCheckTime(int basePeriod);
  bool isActive();
  void setActive(bool active);
};
```

### 8.2 Scheduler Implementation

The scheduler uses TimerOne for precise timing:

```cpp
void Scheduler::init(int basePeriod) {
  this->basePeriod = basePeriod;
  Timer1.initialize(1000L * basePeriod);  // Convert to microseconds
  Timer1.attachInterrupt(timerHandler);
}

void Scheduler::schedule() {
  while (!timerFlag) {}  // Wait for timer interrupt
  timerFlag = false;

  for (int i = 0; i < nTasks; i++) {
    if (taskList[i]->isActive() &&
        taskList[i]->updateAndCheckTime(basePeriod)) {
      taskList[i]->tick();
    }
  }
}
```

### 8.3 Hardware Abstraction

Each hardware component is wrapped in a Device class:

| Class | Hardware | Key Methods |
|-------|----------|-------------|
| `Button` | Tactile switch | `isPressed()` |
| `Led` | LED | `on()`, `off()`, `toggle()` |
| `ServoMotor` | Servo | `open()`, `close()` |
| `UltrasonicSensor` | HC-SR04 | `getDistance()` |
| `PIRSensor` | PIR | `isMotionDetected()` |
| `TempSensorLM35` | LM35 | `getTemperature()` |
| `LCD` | I2C LCD | `print()`, `clear()` |

---

## 9. Breadboard Schema

The circuit schematic and breadboard layout are available on TinkerCad:

**TinkerCad Simulation**: [https://www.tinkercad.com/things/21TEnmh27py/editel](https://www.tinkercad.com/things/21TEnmh27py/editel)

### 9.1 Wiring Summary

```
                         Arduino UNO
                    ┌─────────────────┐
                    │                 │
    [L1 Green LED]──┤ D13         5V ├──┬──[VCC PIR]
                    │                 │  ├──[VCC Ultrasonic]
    [L2 Green LED]──┤ D12        GND ├──┴──[GND all sensors]
                    │                 │
    [L3 Red LED]────┤ D11        A5  ├──[LCD SCL]
                    │                 │
    [Servo Signal]──┤ D9         A4  ├──[LCD SDA]
                    │                 │
    [Ultra Echo]────┤ D5         A1  ├──[LM35 Vout]
                    │                 │
    [Ultra Trig]────┤ D4         A0  ├
                    │                 │
    [PIR Signal]────┤ D3             │
                    │                 │
    [Button]────────┤ D2             │
                    │                 │
                    └─────────────────┘
```

---

## 10. Demo Video

**Video demonstration**: [Link to video on UNIBO OneDrive]

<!-- TODO: Add link to demonstration video -->

The video demonstrates:
1. System startup with drone inside
2. Take-off sequence triggered from DRU
3. Drone exit detection and door closure
4. Landing sequence with PIR detection
5. Distance monitoring during landing
6. Temperature alarm triggering and reset

---

## Appendix A: Build Instructions

### Arduino (PlatformIO)

```bash
cd Assignment_2

# Build
pio run

# Upload to Arduino
pio run --target upload

# Monitor serial output
pio device monitor

# Combined upload and monitor
pio run --target upload && pio device monitor
```

### PC Application (Maven/JavaFX)

```bash
cd Assignment_2/drone-remote-unit

# Build
mvn clean package

# Run
mvn javafx:run
```

---

## Appendix B: Dependencies

### Arduino Libraries

| Library | Version | Purpose |
|---------|---------|---------|
| TimerOne | ^1.2 | Hardware timer for scheduler |
| LiquidCrystal_I2C | ^1.1.4 | I2C LCD driver |

### Java Dependencies

| Dependency | Purpose |
|------------|---------|
| JavaFX 17+ | GUI framework |
| jSerialComm | Serial port communication |

---