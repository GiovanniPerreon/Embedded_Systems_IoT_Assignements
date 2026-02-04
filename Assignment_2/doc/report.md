# Smart Drone Hangar - Assignment 02

---

## 1. Introduction

The **Smart Drone Hangar** is an embedded prototype that manages a drone hangar with automated door control, presence detection, distance monitoring, and temperature-based safety features.

The system consists of two subsystems:
- **Drone Hangar (Arduino UNO)**: Controls sensors, actuators, and operator display
- **Drone Remote Unit (PC/JavaFX)**: GUI to simulate drone commands

## 2. Finite State Machines

### 2.1 DroneHangarTask FSM (7 states)

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

### 2.2 TempTask FSM (5 states)

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
    │                                    └─▲─────┬───────┘
    │                         temp < Temp2 │     │ temp ≥ Temp2
    │                                    ┌─┴─────▼───────┐
    │                                    │  VERYHIGHTEMP │
    │                                    │  Start T4     │
    │                                    └───────┬───────┘
    │                                            │ T4 elapsed
    │              RESET button          ┌───────▼───────┐
    └────────────────────────────────────│     ALARM     │
                                         │ L3 ON, close  │
                                         └───────────────┘
```

## 3. Breadboard Schema

**TinkerCad Simulation:** [https://www.tinkercad.com/things/21TEnmh27py/editel](https://www.tinkercad.com/things/21TEnmh27py/editel)

---
