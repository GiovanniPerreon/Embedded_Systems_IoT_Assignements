# Smart Drone Hangar - Assignment 02

**Embedded Systems and IoT - ISI LT - a.y. 2025/2026**

---

## 1. Introduction

The **Smart Drone Hangar** is an embedded prototype that manages a drone hangar with automated door control, presence detection, distance monitoring, and temperature-based safety features.

The system consists of two subsystems:
- **Drone Hangar (Arduino UNO)**: Controls sensors, actuators, and operator display
- **Drone Remote Unit (PC/JavaFX)**: GUI to simulate drone commands

---

## 2. Hardware Components

| Component | Type | Pin | Function |
|-----------|------|-----|----------|
| L1 | Green LED | D13 | Drone inside indicator |
| L2 | Green LED | D12 | Take-off/landing in progress |
| L3 | Red LED | D11 | Alarm indicator |
| HD | Servo Motor | D9 | Hangar door (0°=closed, 90°=open) |
| DDD | HC-SR04 | D4/D5 | Distance measurement |
| DPD | PIR Sensor | D3 | Motion detection |
| RESET | Button | D2 | Alarm reset |
| TEMP | LM35 | A1 | Temperature monitoring |
| LCD | I2C 16x2 | A4/A5 | Operator display |

---

## 3. Software Architecture

The system uses a **task-based cooperative scheduler** with TimerOne hardware timer (100ms base period).

| Task | Period | Description |
|------|--------|-------------|
| DroneHangarTask | 100ms | Main FSM logic |
| TempTask | 1000ms | Temperature monitoring FSM |
| ButtonTask | 50ms | Debounced button input |
| ServoTask | 100ms | Door control |
| PIRTask | 100ms | Motion detection |
| LCDTask | 200ms | Display updates |
| UltrasonicTask | 1000ms | Distance measurement |
| BlinkTask | 500ms | LED control |

---

## 4. Finite State Machines

### 4.1 DroneHangarTask FSM (7 states)

```
                              TAKEOFF cmd
                           (not in pre-alarm)
                    ┌──────────────────────────────────────┐
                    │                                      │
                    │                                      ▼
            ┌───────┴───────┐                     ┌───────────────┐
            │ DRONE_INSIDE  │                     │   TAKE_OFF    │
            │               │                     │  Door OPEN    │
            │  L1 ON        │                     │  L2 BLINK     │
            │  Door CLOSED  │                     └───────┬───────┘
            └───────────────┘                             │ dist > D1
                    ▲                                     ▼
                    │                             ┌───────────────┐
                    │      T1 elapsed             │ TAKE_OFF_CHECK│
                    │◄────────────────────────────│  Verify exit  │
                    │                             └───────┬───────┘
                    │                                     │ dist < D1
                    │         ┌───────────────────────────┘
                    │         ▼
            ┌───────┴───────┐
            │   DRONE_OUT   │◄────── T1 elapsed ──────────┘
            │  Door CLOSED  │
            └───────┬───────┘
                    │ LAND cmd
                    ▼
            ┌───────────────┐
            │    DETECT     │  Wait for PIR
            └───────┬───────┘
                    │ PIR motion
                    ▼
            ┌───────────────┐      dist > D2     ┌───────────────┐
            │    LANDING    │◄───────────────────│ LANDING_CHECK │
            │  Door OPEN    │                    │  Verify land  │
            │  Send DIST    ├───────────────────►│               │
            └───────────────┘      dist < D2     └───────┬───────┘
                    ▲                                    │ T2 elapsed
                    └────────────────────────────────────┘
                                        │
                                        ▼
                               [Back to DRONE_INSIDE]
```

| State | Actions |
|-------|---------|
| DRONE_INSIDE | L1 ON, door closed, LCD "DRONE INSIDE" |
| TAKE_OFF | Door open, L2 blink, monitor distance |
| TAKE_OFF_CHECK | Verify distance > D1 for T1 ms |
| DRONE_OUT | Door closed, await LAND command |
| DETECT | Wait for PIR motion detection |
| LANDING | Door open, L2 blink, send distance to PC |
| LANDING_CHECK | Verify distance < D2 for T2 ms |

---

### 4.2 TempTask FSM (5 states)

```
                     temp ≥ Temp1
    ┌────────────────────────────────────────────┐
    │                                            ▼
┌───┴───────┐     temp < Temp1           ┌───────────────┐
│  NORMAL   │◄───────────────────────────│   HIGHTEMP    │
│ Valve=0%  │                            │  Start T3     │
└───────────┘                            └───────┬───────┘
    ▲                                            │ T3 elapsed
    │         temp < Temp1               ┌───────▼───────┐
    │◄───────────────────────────────────│   PREALARM    │
    │                                    │ Block new ops │
    │                                    └───────┬───────┘
    │                                            │ temp ≥ Temp2
    │         temp < Temp2               ┌───────▼───────┐
    │◄───────────────────────────────────│  VERYHIGHTEMP │
    │              (via PREALARM)        │  Start T4     │
    │                                    └───────┬───────┘
    │                                            │ T4 elapsed
    │              RESET button          ┌───────▼───────┐
    └────────────────────────────────────│     ALARM     │
                                         │ L3 ON, close  │
                                         └───────────────┘
```

| State | Condition | System Impact |
|-------|-----------|---------------|
| NORMAL | T < Temp1 | All operations allowed |
| HIGHTEMP | T ≥ Temp1 | Timer T3 running |
| PREALARM | T ≥ Temp1 for T3 | New operations blocked |
| VERYHIGHTEMP | T ≥ Temp2 | Timer T4 running |
| ALARM | T ≥ Temp2 for T4 | All ops suspended, door closed, L3 ON |

---

## 5. Serial Protocol

**Baud rate:** 9600

| Direction | Command | Description |
|-----------|---------|-------------|
| PC→Arduino | `TAKEOFF` | Request take-off |
| PC→Arduino | `LAND` | Request landing |
| Arduino→PC | `INSIDE/OUTSIDE/TAKEOFF/LANDING` | Drone state |
| Arduino→PC | `NORMAL/PREALARM/ALARM` | Temperature state |
| Arduino→PC | `DIST:<value>` | Distance during landing |

---

## 6. Configuration Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| D1 | 50 cm | Exit confirmation distance |
| D2 | 30 cm | Landing confirmation distance |
| T1, T2 | 3000 ms | Confirmation time windows |
| T3, T4 | 3000 ms | Temperature alarm timers |
| Temp1 | 20°C | Pre-alarm threshold |
| Temp2 | 25°C | Alarm threshold |

---

## 7. Breadboard Schema

**TinkerCad Simulation:** [https://www.tinkercad.com/things/21TEnmh27py/editel](https://www.tinkercad.com/things/21TEnmh27py/editel)

---

## 8. Build & Run

```bash
cd Assignment_2
pio run --target upload && pio device monitor
```

**Java GUI:**
```bash
cd drone-remote-unit && mvn javafx:run
```

---

*Embedded Systems and IoT - ISI LT - a.y. 2025/2026*
