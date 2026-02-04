# Smart Tank Monitoring System - Assignment 03

**Embedded Systems and IoT - ISI LT - a.y. 2025/2026**

---

## 1. Introduction

The **Smart Tank Monitoring System** is an IoT prototype for monitoring rainwater levels and automatically controlling a drainage valve based on configurable thresholds.

The system comprises four subsystems:
- **TMS (ESP32)**: Water level sensor, publishes to MQTT
- **CUS (Java/PC)**: Control unit, policy engine, coordinates all subsystems
- **WCS (Arduino UNO)**: Valve controller with local operator panel
- **DBS (Web App)**: Dashboard for visualization and remote control

---

## 2. Finite State Machines

### 2.1 TMS FSM (Connection Status)

```
                WiFi + MQTT connected
         ┌──────────────────────────────────────┐
         │                                      ▼
  ┌──────┴───────┐                      ┌───────────────┐
  │ DISCONNECTED │                      │   CONNECTED   │
  │  Red ON      │                      │  Green ON     │
  │  No publish  │                      │  Publish F ms │
  └──────────────┘                      └───────┬───────┘
         ▲                                      │
         └──────── WiFi/MQTT lost ──────────────┘
```

### 2.2 WCS FSM (Mode Control - 3 states)

```
                              MODE_AUTO cmd / Button
            ┌─────────────────────────────────────────────┐
            │                                             │
            ▼                                             │
    ┌───────────────┐       MODE_MANUAL / Button    ┌─────┴───────┐
    │   AUTOMATIC   │◄─────────────────────────────►│    MANUAL   │
  ┌►│               │                               │             │
  │ │ Valve by CUS  │                               │ Valve by Pot│
  │ │ Pot ignored   │                               │ or Dashboard│
  │ └───────┬───────┘                               └──────┬──────┘
  │         │                                              │
  │         │  UNCONNECTED cmd                             │
  │         │  (no MQTT for T2)                            │
  │         ▼                                              │
  │ ┌───────────────┐                                      │
  │ │  UNCONNECTED  │◄─────────────────────────────────────┘
  │ │  Valve = 0%   │         UNCONNECTED cmd
  │ │Button disabled│
  │ └───────┬───────┘
  │         │ MQTT data resumes
  └─────────┘
```

## 3. Breadboard Schema

| Subsystem | Simulation |
|-----------|------------|
| WCS (Arduino) | [TinkerCad](https://www.tinkercad.com/things/iggcpeCcoL2-assignment-3/editel) |
| TMS (ESP32) | [Wokwi](https://wokwi.com/projects/454858190991922177) |

---
