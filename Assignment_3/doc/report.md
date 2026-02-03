# Smart Tank Monitoring System - Assignment 03

**Embedded Systems and IoT - ISI LT - a.y. 2025/2026**

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [System Overview](#2-system-overview)
3. [Hardware Architecture](#3-hardware-architecture)
4. [Software Architecture](#4-software-architecture)
5. [Finite State Machines](#5-finite-state-machines)
6. [Communication Protocols](#6-communication-protocols)
7. [Water Level Policy](#7-water-level-policy)
8. [Configuration Parameters](#8-configuration-parameters)
9. [Implementation Details](#9-implementation-details)
10. [Breadboard Schema](#10-breadboard-schema)
11. [Demo Video](#11-demo-video)

---

## 1. Introduction

This report describes the design and implementation of the **Smart Tank Monitoring System**, an IoT prototype for monitoring rainwater levels in a tank and automatically controlling the drainage valve based on configurable thresholds.

The system comprises four cooperating subsystems:
- **TMS (Tank Monitoring Subsystem)** - ESP32-based water level sensor
- **CUS (Control Unit Subsystem)** - Java backend coordinating all subsystems
- **WCS (Water Channel Subsystem)** - Arduino-based valve controller
- **DBS (Dashboard Subsystem)** - Web-based operator interface

### 1.1 Key Features

- Real-time water level monitoring via ultrasonic sensor
- Automatic valve control based on configurable thresholds
- Manual override capability via physical controls or web dashboard
- Network fault tolerance with UNCONNECTED state handling
- Historical data visualization with configurable history depth

---

## 2. System Overview

### 2.1 Subsystem Responsibilities

| Subsystem | Platform | Responsibilities |
|-----------|----------|------------------|
| **TMS** | ESP32 | Monitor water level, publish to MQTT, indicate connection status |
| **CUS** | Java/PC | Policy evaluation, subsystem coordination, HTTP API |
| **WCS** | Arduino UNO | Valve control, local operator panel, mode switching |
| **DBS** | Web App | Data visualization, remote control interface |

### 2.2 System Architecture Diagram

```
┌──────────────────────────────────────────────────────────────────────────────────────┐
│                         SMART TANK MONITORING SYSTEM                                  │
├──────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                      │
│    ┌────────────────────────┐                      ┌────────────────────────┐       │
│    │   TMS (ESP32)          │        MQTT          │    CUS (Java/PC)       │       │
│    │                        │    tank/level        │                        │       │
│    │  ┌──────────────────┐  │  ┌──────────────┐   │  ┌──────────────────┐  │       │
│    │  │ Ultrasonic Sonar │──┼──►│ {"level":X} │───┼──►│  MQTT Handler    │  │       │
│    │  └──────────────────┘  │  └──────────────┘   │  └────────┬─────────┘  │       │
│    │                        │                      │           │            │       │
│    │  ┌────────┐ ┌────────┐ │                      │  ┌────────▼─────────┐  │       │
│    │  │Green   │ │Red     │ │                      │  │  System State    │  │       │
│    │  │LED     │ │LED     │ │                      │  │  (mode, level,   │  │       │
│    │  │(OK)    │ │(ERROR) │ │                      │  │   valve, history)│  │       │
│    │  └────────┘ └────────┘ │                      │  └────────┬─────────┘  │       │
│    │                        │                      │           │            │       │
│    └────────────────────────┘                      │  ┌────────▼─────────┐  │       │
│                                                    │  │ Water Level      │  │       │
│                                                    │  │ Policy Engine    │  │       │
│                                                    │  └────────┬─────────┘  │       │
│                                                    │           │            │       │
│                                                    │  ┌────────┴─────────┐  │       │
│                                                    │  │                  │  │       │
│                                                    │  ▼                  ▼  │       │
│                                                    │ Serial           HTTP  │       │
│                                                    │ Handler          Server│       │
│                                                    └───┬──────────────┬────┘       │
│                                                        │              │            │
│                           Serial (9600 baud)           │              │ HTTP:8080  │
│                                    ┌───────────────────┘              │            │
│                                    │                                  │            │
│                                    ▼                                  ▼            │
│    ┌────────────────────────┐                      ┌────────────────────────┐      │
│    │   WCS (Arduino UNO)    │                      │    DBS (Web App)       │      │
│    │                        │                      │                        │      │
│    │  ┌──────────────────┐  │                      │  ┌──────────────────┐  │      │
│    │  │ Servo Motor      │  │                      │  │ Water Level      │  │      │
│    │  │ (Valve 0-90°)    │  │                      │  │ Chart (N points) │  │      │
│    │  └──────────────────┘  │                      │  └──────────────────┘  │      │
│    │                        │                      │                        │      │
│    │  ┌──────────────────┐  │                      │  ┌──────────────────┐  │      │
│    │  │ LCD Display      │  │                      │  │ Valve Control    │  │      │
│    │  │ (Mode + Valve %) │  │                      │  │ Slider           │  │      │
│    │  └──────────────────┘  │                      │  └──────────────────┘  │      │
│    │                        │                      │                        │      │
│    │  ┌────────┐ ┌────────┐ │                      │  ┌──────────────────┐  │      │
│    │  │Button  │ │Potent. │ │                      │  │ Mode Toggle      │  │      │
│    │  │(Mode)  │ │(Manual)│ │                      │  │ (AUTO/MANUAL)    │  │      │
│    │  └────────┘ └────────┘ │                      │  └──────────────────┘  │      │
│    │                        │                      │                        │      │
│    └────────────────────────┘                      └────────────────────────┘      │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

### 2.3 Data Flow

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                              DATA FLOW DIAGRAM                                       │
├─────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                     │
│   SENSOR DATA FLOW (TMS → CUS → DBS)                                               │
│   ════════════════════════════════════                                              │
│                                                                                     │
│   [Sonar] ──► [TMS] ══MQTT══► [CUS] ══HTTP══► [DBS Chart]                          │
│               {"level":42.5}   Store in      GET /api/state                         │
│                                history[N]    Returns JSON                           │
│                                                                                     │
│   CONTROL FLOW (CUS → WCS)                                                          │
│   ════════════════════════                                                          │
│                                                                                     │
│   [Policy] ──► [CUS] ══Serial══► [WCS] ──► [Servo]                                 │
│   Evaluate     VALVE_OPEN:50     Set angle   0-90°                                  │
│   thresholds                     Update LCD                                         │
│                                                                                     │
│   MANUAL CONTROL FLOW                                                               │
│   ═══════════════════                                                               │
│                                                                                     │
│   Option A: Physical                                                                │
│   [Button] ──► [WCS] ══Serial══► [CUS] ──► Update state                            │
│   [Pot]    ──► MODE_MANUAL       MANUAL                                             │
│                VALVE_OPEN:X      VALVE_OPEN:X                                       │
│                                                                                     │
│   Option B: Remote                                                                  │
│   [DBS] ══HTTP══► [CUS] ══Serial══► [WCS]                                          │
│   POST /api/mode  MODE_MANUAL       Update                                          │
│   POST /api/valve VALVE_OPEN:X      servo+LCD                                       │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 3. Hardware Architecture

### 3.1 TMS Components (ESP32)

| Component | Type | GPIO | Function |
|-----------|------|------|----------|
| **Sonar** | HC-SR04 Ultrasonic | Trig=5, Echo=18 | Water level measurement |
| **Green LED** | LED | GPIO 19 | Network OK indicator |
| **Red LED** | LED | GPIO 21 | Network error indicator |

### 3.2 WCS Components (Arduino UNO)

| Component | Type | Pin | Function |
|-----------|------|-----|----------|
| **Servo** | SG90 Servo Motor | D9 | Valve control (0°=closed, 90°=full open) |
| **Button** | Tactile Switch | D2 | Mode toggle (AUTO↔MANUAL) |
| **Potentiometer** | 10kΩ Rotary | A0 | Manual valve control (0-100%) |
| **LCD** | I2C LCD 16x2 | A4 (SDA), A5 (SCL) | Display mode and valve % |
| **Green LED** | LED | D6 | Connection OK |
| **Red LED** | LED | D7 | Connection error |

### 3.3 Pin Diagrams

**ESP32 (TMS):**
```
                         ESP32 DevKit
                    ┌─────────────────────┐
                    │                     │
    [Sonar Trig]────┤ GPIO5          3V3 ├────[Sonar VCC]
                    │                     │
    [Sonar Echo]────┤ GPIO18         GND ├────[Sonar GND]
                    │                     │
    [Green LED]─────┤ GPIO19             │
                    │                     │
    [Red LED]───────┤ GPIO21             │
                    │                     │
                    │        [USB]        │
                    └─────────────────────┘
```

**Arduino UNO (WCS):**
```
                         Arduino UNO
                    ┌─────────────────────┐
                    │                     │
    [Servo Signal]──┤ D9              5V ├────[Servo VCC, LCD VCC]
                    │                     │
    [Button]────────┤ D2             GND ├────[Common GND]
                    │                     │
    [Green LED]─────┤ D6              A4 ├────[LCD SDA]
                    │                     │
    [Red LED]───────┤ D7              A5 ├────[LCD SCL]
                    │                     │
                    │                 A0 ├────[Potentiometer]
                    │                     │
                    │        [USB]        │
                    └─────────────────────┘
```

---

## 4. Software Architecture

### 4.1 TMS Architecture (ESP32)

The TMS uses a **task-based architecture** with periodic execution:

```
┌─────────────────────────────────────────────────────────────────┐
│                      TMS SOFTWARE ARCHITECTURE                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │                      main.cpp                            │  │
│   │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────┐  │  │
│   │  │ WiFi Setup  │  │ MQTT Setup  │  │ Scheduler Init  │  │  │
│   │  └─────────────┘  └─────────────┘  └─────────────────┘  │  │
│   └─────────────────────────────────────────────────────────┘  │
│                              │                                  │
│                              ▼                                  │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │                    Scheduler (100ms)                     │  │
│   │  ┌─────────────────────────────────────────────────┐    │  │
│   │  │                   TmsTask                        │    │  │
│   │  │                                                  │    │  │
│   │  │  • Read ultrasonic sensor                        │    │  │
│   │  │  • Check MQTT connection status                  │    │  │
│   │  │  • Update LED indicators                         │    │  │
│   │  │  • Publish level JSON every F ms                 │    │  │
│   │  │                                                  │    │  │
│   │  └─────────────────────────────────────────────────┘    │  │
│   │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │  │
│   │  │ BlinkTask    │  │ BlinkTask    │  │ Ultrasonic   │   │  │
│   │  │ (Green LED)  │  │ (Red LED)    │  │ Task         │   │  │
│   │  └──────────────┘  └──────────────┘  └──────────────┘   │  │
│   └─────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 4.2 WCS Architecture (Arduino UNO)

The WCS implements a **3-state FSM** with task-based peripheral handling:

```
┌─────────────────────────────────────────────────────────────────┐
│                      WCS SOFTWARE ARCHITECTURE                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │                    Scheduler (100ms)                     │  │
│   │                                                          │  │
│   │  ┌─────────────────────────────────────────────────┐    │  │
│   │  │                    WcsTask                       │    │  │
│   │  │                                                  │    │  │
│   │  │  ┌─────────────────────────────────────────┐    │    │  │
│   │  │  │         FSM (3 States)                  │    │    │  │
│   │  │  │  UNCONNECTED ←→ AUTOMATIC ←→ MANUAL     │    │    │  │
│   │  │  └─────────────────────────────────────────┘    │    │  │
│   │  │                                                  │    │  │
│   │  │  • Handle serial commands from CUS               │    │  │
│   │  │  • Process button presses (mode toggle)          │    │  │
│   │  │  • Read potentiometer (manual mode)              │    │  │
│   │  │  • Control servo position                        │    │  │
│   │  │  • Update LCD display                            │    │  │
│   │  │                                                  │    │  │
│   │  └─────────────────────────────────────────────────┘    │  │
│   │                                                          │  │
│   │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │  │
│   │  │ Button   │ │ Servo    │ │ LCD      │ │ LED      │   │  │
│   │  │ Task     │ │ Task     │ │ Task     │ │ Tasks    │   │  │
│   │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │  │
│   │                                                          │  │
│   └─────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 4.3 CUS Architecture (Java)

The CUS is a **multi-threaded server** coordinating all subsystems:

```
┌─────────────────────────────────────────────────────────────────┐
│                      CUS SOFTWARE ARCHITECTURE                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │                    ControlUnit.java                      │  │
│   │                     (Main Entry)                         │  │
│   └────────────────────────┬────────────────────────────────┘  │
│                            │                                    │
│            ┌───────────────┼───────────────┐                   │
│            │               │               │                    │
│            ▼               ▼               ▼                    │
│   ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│   │ MqttHandler │  │SerialHandler│  │ HttpServer  │           │
│   │  (Thread)   │  │  (Thread)   │  │  (Thread)   │           │
│   │             │  │             │  │             │           │
│   │ Subscribe   │  │ Send cmds   │  │ REST API    │           │
│   │ tank/level  │  │ Read resp   │  │ /api/state  │           │
│   │             │  │             │  │ /api/mode   │           │
│   │             │  │             │  │ /api/valve  │           │
│   └──────┬──────┘  └──────┬──────┘  └──────┬──────┘           │
│          │                │                │                    │
│          └────────────────┼────────────────┘                   │
│                           ▼                                     │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │                    SystemState.java                      │  │
│   │                                                          │  │
│   │  • mode: AUTOMATIC | MANUAL | UNCONNECTED               │  │
│   │  • latestWaterLevel: double                              │  │
│   │  • valveOpenPercent: int                                 │  │
│   │  • levelHistory: CircularBuffer[N]                       │  │
│   │  • lastMqttTimestamp: long                               │  │
│   │  • l1ExceededSince: long                                 │  │
│   │                                                          │  │
│   └────────────────────────┬────────────────────────────────┘  │
│                            │                                    │
│                            ▼                                    │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │                 WaterLevelPolicy.java                    │  │
│   │                     (Thread)                             │  │
│   │                                                          │  │
│   │  Every 500ms:                                            │  │
│   │  1. Check UNCONNECTED condition (no MQTT for T2)         │  │
│   │  2. If AUTOMATIC mode, evaluate thresholds:              │  │
│   │     - level > L2 → valve 100%                            │  │
│   │     - level > L1 for T1 → valve 50%                      │  │
│   │     - level ≤ L1 → valve 0%                              │  │
│   │  3. Send commands to WCS via SerialHandler               │  │
│   │                                                          │  │
│   └─────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 4.4 DBS Architecture (Web Frontend)

```
┌─────────────────────────────────────────────────────────────────┐
│                      DBS SOFTWARE ARCHITECTURE                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │                     index.html                           │  │
│   │  ┌─────────────────────────────────────────────────┐    │  │
│   │  │              Water Level Chart                   │    │  │
│   │  │         (Chart.js, last N readings)              │    │  │
│   │  └─────────────────────────────────────────────────┘    │  │
│   │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │  │
│   │  │ Mode Display │  │ Valve %      │  │ Status       │   │  │
│   │  │ + Toggle Btn │  │ + Slider     │  │ Indicator    │   │  │
│   │  └──────────────┘  └──────────────┘  └──────────────┘   │  │
│   └─────────────────────────────────────────────────────────┘  │
│                              │                                  │
│                              ▼                                  │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │                    dashboard.js                          │  │
│   │                                                          │  │
│   │  setInterval(pollState, 1000):                          │  │
│   │    GET /api/state → update chart, mode, valve           │  │
│   │                                                          │  │
│   │  onModeToggle():                                         │  │
│   │    POST /api/mode {mode: "MANUAL"|"AUTOMATIC"}          │  │
│   │                                                          │  │
│   │  onValveChange():                                        │  │
│   │    POST /api/valve {percent: 0-100}                     │  │
│   │                                                          │  │
│   └─────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 5. Finite State Machines

### 5.1 TMS FSM (Connection Status)

The TMS implements a simple 2-state FSM for connection status indication:

```
┌─────────────────────────────────────────────────────────────────┐
│                         TMS FSM                                  │
│                   (Connection Status)                            │
└─────────────────────────────────────────────────────────────────┘

                    WiFi + MQTT connected
         ┌──────────────────────────────────────────┐
         │                                          │
         │                                          ▼
  ┌──────┴───────┐                          ┌───────────────┐
  │              │                          │               │
  │ DISCONNECTED │                          │   CONNECTED   │
  │              │                          │               │
  │  • Red ON    │                          │  • Green ON   │
  │  • Green OFF │                          │  • Red OFF    │
  │  • No publish│                          │  • Publish    │
  │              │                          │    every F ms │
  │              │                          │               │
  └──────────────┘                          └───────┬───────┘
         ▲                                          │
         │                                          │
         └──────────────────────────────────────────┘
                    WiFi or MQTT lost
```

#### State Descriptions

| State | LED Status | MQTT | Description |
|-------|------------|------|-------------|
| `CONNECTED` | Green ON, Red OFF | Publishing | Normal operation, sending water level readings |
| `DISCONNECTED` | Red ON, Green OFF | Not publishing | Network or MQTT broker unreachable |

---

### 5.2 WCS FSM (Mode Control)

The WCS implements a 3-state FSM for mode management:

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                    WCS FSM                                           │
│                               (Mode Control)                                         │
└─────────────────────────────────────────────────────────────────────────────────────┘

                                 CUS sends MODE_AUTO
                              or Button press in MANUAL
            ┌───────────────────────────────────────────────────────┐
            │                                                       │
            │                                                       │
            ▼                                                       │
    ┌───────────────┐        CUS sends MODE_MANUAL           ┌──────┴────────┐
    │               │        or Button press in AUTO         │               │
    │   AUTOMATIC   │◄──────────────────────────────────────►│    MANUAL     │
    │               │                                        │               │
    │  • Valve set  │                                        │  • Valve set  │
    │    by CUS     │                                        │    by Pot or  │
    │  • LCD shows  │                                        │    Dashboard  │
    │    "AUTOMATIC"│                                        │  • LCD shows  │
    │  • Pot ignored│                                        │    "MANUAL"   │
    │               │                                        │  • Send valve │
    │               │                                        │    changes    │
    └───────┬───────┘                                        └───────┬───────┘
            │                                                        │
            │  CUS sends UNCONNECTED                                 │
            │  (no MQTT data for T2)                                 │
            │                                                        │
            │        ┌───────────────────────────────────────────────┘
            │        │  CUS sends UNCONNECTED
            ▼        ▼
    ┌───────────────────────┐
    │                       │
    │     UNCONNECTED       │
    │                       │
    │  • Valve = 0%         │
    │  • LCD shows          │
    │    "UNCONNECTED"      │
    │  • Button disabled    │
    │  • Pot ignored        │
    │  • Wait for CUS       │
    │                       │
    └───────────┬───────────┘
                │
                │  CUS sends MODE_AUTO or MODE_MANUAL
                │  (MQTT data resumed)
                │
                └────────────────────────────────────────────────────►
                                                      (to AUTOMATIC or MANUAL)
```

#### State Descriptions

| State | Valve Control | Button | Potentiometer | LCD Display |
|-------|---------------|--------|---------------|-------------|
| `UNCONNECTED` | Fixed at 0% | Disabled | Ignored | "UNCONNECTED" |
| `AUTOMATIC` | CUS policy | Toggle to MANUAL | Ignored | "AUTOMATIC" |
| `MANUAL` | Pot or Dashboard | Toggle to AUTO | Active (0-100%) | "MANUAL" |

#### State Transition Table

| Current State | Event | Next State | Actions |
|---------------|-------|------------|---------|
| `UNCONNECTED` | CUS: MODE_AUTO | `AUTOMATIC` | Update LCD, respond AUTOMATIC |
| `UNCONNECTED` | CUS: MODE_MANUAL | `MANUAL` | Update LCD, respond MANUAL |
| `AUTOMATIC` | Button press | `MANUAL` | Send MODE_MANUAL to CUS, update LCD |
| `AUTOMATIC` | CUS: MODE_MANUAL | `MANUAL` | Update LCD, respond MANUAL |
| `AUTOMATIC` | CUS: UNCONNECTED | `UNCONNECTED` | Close valve, update LCD |
| `MANUAL` | Button press | `AUTOMATIC` | Send MODE_AUTO to CUS, update LCD |
| `MANUAL` | CUS: MODE_AUTO | `AUTOMATIC` | Update LCD, respond AUTOMATIC |
| `MANUAL` | CUS: UNCONNECTED | `UNCONNECTED` | Close valve, update LCD |
| `MANUAL` | Pot change | `MANUAL` | Update valve, send VALVE_OPEN:X |
| `MANUAL` | CUS: VALVE_OPEN:X | `MANUAL` | Override pot, set valve to X% |

---

### 5.3 CUS Policy FSM (Water Level Control)

The CUS policy engine implements a 4-state FSM for automatic valve control:

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                CUS POLICY FSM                                        │
│                           (Water Level Control)                                      │
└─────────────────────────────────────────────────────────────────────────────────────┘

                              No MQTT data for T2
    ┌─────────────────────────────────────────────────────────────────────────────┐
    │                                                                             │
    │                                                                             │
    ▼                                                                             │
┌───────────────────┐                                                             │
│                   │                                                             │
│   UNCONNECTED     │         MQTT data resumes                                   │
│                   │◄────────────────────────────────────────────────────────────┤
│  • Valve = 0%     │                                                             │
│  • Send to WCS    │                                                             │
│                   │────────────────────────────────────────────────────────────►│
└───────────────────┘         MQTT data resumes                                   │
                                                                                  │
                                                                                  │
                              level ≤ L1                                          │
         ┌────────────────────────────────────────────────────┐                   │
         │                                                    │                   │
         │                                                    │                   │
         ▼                                                    │                   │
  ┌──────────────┐          level > L1                 ┌──────┴───────┐          │
  │              │          (start T1 timer)           │              │          │
  │    NORMAL    │────────────────────────────────────►│     HIGH     │          │
  │              │                                     │              │          │
  │  • Valve = 0%│          level ≤ L1                 │  • Timer T1  │          │
  │              │◄────────────────────────────────────│    running   │          │
  │              │                                     │  • Valve     │          │
  └──────────────┘                                     │    unchanged │          │
         │                                             └──────┬───────┘          │
         │                                                    │                   │
         │                                                    │ T1 elapsed       │
         │                                                    │ (level > L1      │
         │                                                    │  for T1 ms)      │
         │                                                    ▼                   │
         │                                             ┌──────────────┐          │
         │              level ≤ L1                     │              │          │
         │◄────────────────────────────────────────────│   WARNING    │          │
         │                                             │              │          │
         │                                             │  • Valve=50% │          │
         │                                             │              │          │
         │                                             └──────┬───────┘          │
         │                                                    │                   │
         │                                                    │ level > L2       │
         │                                                    ▼                   │
         │                                             ┌──────────────┐          │
         │              level ≤ L1                     │              │          │
         │◄────────────────────────────────────────────│   CRITICAL   │──────────┘
         │                                             │              │  no MQTT
         │              level ≤ L2                     │ • Valve=100% │  for T2
         │         ┌───────────────────────────────────│              │
         │         │                                   │              │
         │         ▼                                   └──────────────┘
         │  ┌──────────────┐
         │  │   WARNING    │
         │  └──────────────┘
         │
         └─────────────────────────────────────────────────────────────────────────┘
```

#### State Descriptions

| State | Water Level | Valve Position | Description |
|-------|-------------|----------------|-------------|
| `UNCONNECTED` | Unknown | 0% (closed) | No MQTT data received for T2 ms |
| `NORMAL` | ≤ L1 | 0% (closed) | Water level safe, no drainage needed |
| `HIGH` | > L1, ≤ L2 | Unchanged | Timer T1 running, waiting to confirm |
| `WARNING` | > L1 for T1 | 50% (half open) | Moderate drainage to reduce level |
| `CRITICAL` | > L2 | 100% (full open) | Maximum drainage, immediate action |

#### Policy Logic (Pseudocode)

```
every POLICY_EVAL_INTERVAL (500ms):

    // Check UNCONNECTED condition
    if timeSinceLastMqtt() > T2:
        mode = UNCONNECTED
        valve = 0%
        return

    // If was UNCONNECTED and data resumed
    if mode == UNCONNECTED and timeSinceLastMqtt() <= T2:
        mode = AUTOMATIC

    // Only evaluate if in AUTOMATIC mode
    if mode != AUTOMATIC:
        return

    // Apply threshold logic
    if level > L2:
        valve = 100%
        resetL1Timer()

    else if level > L1:
        startL1Timer()
        if l1ExceededDuration >= T1:
            valve = 50%

    else:  // level <= L1
        valve = 0%
        resetL1Timer()
```

---

### 5.4 FSM Interaction Diagram

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                           FSM INTERACTION OVERVIEW                                   │
├─────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                     │
│   TMS FSM                    CUS Policy FSM                    WCS FSM             │
│   ═══════                    ══════════════                    ═══════             │
│                                                                                     │
│   ┌─────────┐                ┌─────────────┐                  ┌─────────────┐      │
│   │CONNECTED│───MQTT────────►│   NORMAL    │───Serial────────►│  AUTOMATIC  │      │
│   │         │  {"level":X}   │   /HIGH/    │  VALVE_OPEN:X    │             │      │
│   │         │                │   WARNING/  │                  │             │      │
│   │         │                │   CRITICAL  │                  │             │      │
│   └────┬────┘                └──────┬──────┘                  └──────┬──────┘      │
│        │                            │                                │             │
│        │ lost                       │ no data                        │ button      │
│        ▼                            │ for T2                         ▼             │
│   ┌─────────┐                       │                         ┌─────────────┐      │
│   │DISCONN- │                       │                         │   MANUAL    │      │
│   │ECTED    │                       ▼                         │             │      │
│   └─────────┘                ┌─────────────┐                  └──────┬──────┘      │
│                              │ UNCONNECTED │───Serial────────►       │             │
│                              │             │  UNCONNECTED            │             │
│                              └─────────────┘                         ▼             │
│                                                               ┌─────────────┐      │
│                                                               │ UNCONNECTED │      │
│                                                               │             │      │
│                                                               └─────────────┘      │
│                                                                                     │
│   Legend:                                                                           │
│   ───────►  Data/Command flow                                                       │
│   ════════  FSM state                                                               │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 6. Communication Protocols

### 6.1 MQTT Protocol (TMS ↔ CUS)

| Parameter | Value |
|-----------|-------|
| Broker | test.mosquitto.org |
| Port | 1883 (unencrypted) |
| Topic | `tank/level` |
| QoS | 1 (at least once) |
| Payload Format | JSON |

**Message Format:**
```json
{
  "level": 42.50
}
```

**Sequence Diagram:**
```
    TMS (ESP32)                    MQTT Broker                    CUS (Java)
        │                              │                              │
        │  CONNECT                     │                              │
        ├─────────────────────────────►│                              │
        │                              │                              │
        │                              │  SUBSCRIBE tank/level        │
        │                              │◄─────────────────────────────┤
        │                              │                              │
        │  PUBLISH tank/level          │                              │
        │  {"level":35.2}              │                              │
        ├─────────────────────────────►│                              │
        │                              │  PUBLISH tank/level          │
        │                              │  {"level":35.2}              │
        │                              ├─────────────────────────────►│
        │                              │                              │
        │         ... every F ms ...   │                              │
        │                              │                              │
```

---

### 6.2 Serial Protocol (CUS ↔ WCS)

| Parameter | Value |
|-----------|-------|
| Baud Rate | 9600 |
| Data Bits | 8 |
| Stop Bits | 1 |
| Parity | None |
| Terminator | `\n` (newline) |

#### Commands (CUS → WCS)

| Command | Description | Response |
|---------|-------------|----------|
| `MODE_MANUAL` | Switch to manual mode | `MANUAL` |
| `MODE_AUTO` | Switch to automatic mode | `AUTOMATIC` |
| `VALVE_OPEN:<n>` | Set valve to n% (0-100) | `VALVE_OPEN:<n>` |
| `STATUS?` | Query current state | Mode + `VALVE_OPEN:<n>` |
| `UNCONNECTED` | Enter unconnected state | `UNCONNECTED` |

#### Responses/Notifications (WCS → CUS)

| Message | Description |
|---------|-------------|
| `MANUAL` | Entered manual mode |
| `AUTOMATIC` | Entered automatic mode |
| `UNCONNECTED` | Entered unconnected state |
| `VALVE_OPEN:<n>` | Current valve position (0-100%) |

#### Sequence Diagrams

**Automatic Mode - Valve Control:**
```
    CUS (Java)                                    WCS (Arduino)
        │                                              │
        │  ────── "VALVE_OPEN:50\n" ──────────────►   │
        │                                              │  [Set servo to 45°]
        │  ◄────── "VALVE_OPEN:50\n" ──────────────   │  [Update LCD]
        │                                              │
```

**Manual Mode - Button Toggle:**
```
    CUS (Java)                                    WCS (Arduino)
        │                                              │
        │                                              │  [Button pressed]
        │  ◄────── "MODE_MANUAL\n" ────────────────   │
        │                                              │
        │         ... user adjusts pot ...             │
        │                                              │
        │  ◄────── "VALVE_OPEN:75\n" ──────────────   │
        │                                              │
```

**Unconnected State:**
```
    CUS (Java)                                    WCS (Arduino)
        │                                              │
        │  [No MQTT for T2 ms]                         │
        │                                              │
        │  ────── "UNCONNECTED\n" ────────────────►   │
        │                                              │  [Close valve]
        │  ◄────── "UNCONNECTED\n" ────────────────   │  [LCD: UNCONNECTED]
        │                                              │
```

---

### 6.3 HTTP REST API (CUS ↔ DBS)

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/state` | GET | Get current system state |
| `/api/mode` | POST | Set system mode |
| `/api/valve` | POST | Set valve position (manual mode only) |

#### GET /api/state

**Response:**
```json
{
  "mode": "AUTOMATIC",
  "waterLevel": 35.2,
  "valvePercent": 0,
  "history": [32.1, 33.5, 34.2, 35.2],
  "timestamp": 1699876543210
}
```

| Field | Type | Description |
|-------|------|-------------|
| `mode` | string | `AUTOMATIC`, `MANUAL`, or `UNCONNECTED` |
| `waterLevel` | number | Latest water level reading (cm) |
| `valvePercent` | number | Current valve opening (0-100) |
| `history` | array | Last N water level readings |
| `timestamp` | number | Unix timestamp (ms) |

#### POST /api/mode

**Request:**
```json
{
  "mode": "MANUAL"
}
```

**Response:**
```json
{
  "success": true,
  "mode": "MANUAL"
}
```

#### POST /api/valve

**Request:**
```json
{
  "percent": 75
}
```

**Response:**
```json
{
  "success": true,
  "percent": 75
}
```

---

## 7. Water Level Policy

### 7.1 Threshold-Based Control

The automatic control policy uses two thresholds (L1, L2) and two timers (T1, T2):

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                           WATER LEVEL THRESHOLDS                                     │
├─────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                     │
│   Water Level (cm)                                                                  │
│        ▲                                                                            │
│        │                                                                            │
│   100 ─┤                                              ┌─────────────────────────    │
│        │                                              │  OVERFLOW DANGER            │
│        │                                              │                             │
│    L2 ─┼──────────────────────────────────────────────┤  ═══════════════════════   │
│  (50) ─┤                                              │  CRITICAL: Valve 100%       │
│        │                              ┌───────────────┤  (immediate)                │
│        │                              │               │                             │
│    L1 ─┼──────────────────────────────┤               │  ═══════════════════════   │
│  (30) ─┤                              │               │  WARNING: Valve 50%         │
│        │              ┌───────────────┤               │  (after T1 = 5s)            │
│        │              │               │               │                             │
│        │              │               │               │  ═══════════════════════   │
│     0 ─┼──────────────┤               │               │  NORMAL: Valve 0%           │
│        │              │               │               │                             │
│        └──────────────┴───────────────┴───────────────┴─────────────────────────►  │
│                       │               │               │                  Time       │
│                    NORMAL          WARNING         CRITICAL                         │
│                  Valve=0%         Valve=50%       Valve=100%                        │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

### 7.2 Policy Rules

| Condition | Timer | Valve Action | Rationale |
|-----------|-------|--------------|-----------|
| Level ≤ L1 | - | 0% (closed) | Safe level, no drainage needed |
| L1 < Level ≤ L2 | Wait T1 | 50% (half) | Moderate level, gradual drainage |
| Level > L2 | Immediate | 100% (full) | Critical level, maximum drainage |
| No MQTT for T2 | - | 0% + UNCONNECTED | Safety: close valve if sensor lost |

### 7.3 Hysteresis Behavior

To prevent valve oscillation near thresholds:

```
Level rising:   L1 ────────► triggers at L1
                        │
                        ▼
Level falling:  L1 ◄──────── releases at L1
                        │
                        │  (no hysteresis band in current impl)
                        ▼
```

**Note:** The current implementation uses exact thresholds without hysteresis. For production, consider adding a hysteresis band (e.g., ±2cm) to prevent rapid valve cycling.

---

## 8. Configuration Parameters

### 8.1 CUS Configuration (Config.java)

| Parameter | Value | Unit | Description |
|-----------|-------|------|-------------|
| `L1` | 30.0 | cm | First threshold (50% valve) |
| `L2` | 50.0 | cm | Second threshold (100% valve) |
| `T1` | 5000 | ms | Time before L1 triggers valve |
| `T2` | 10000 | ms | Timeout for UNCONNECTED state |
| `N` | 50 | readings | History buffer size for dashboard |
| `POLICY_EVAL_INTERVAL` | 500 | ms | Policy evaluation frequency |
| `HTTP_PORT` | 8080 | - | Dashboard API port |
| `MQTT_BROKER` | tcp://test.mosquitto.org:1883 | - | MQTT broker URL |
| `MQTT_TOPIC` | tank/level | - | MQTT topic for water levels |
| `SERIAL_PORT` | COM3 | - | Serial port for WCS |
| `SERIAL_BAUD` | 9600 | baud | Serial communication speed |

### 8.2 TMS Configuration (config.h)

| Parameter | Value | Unit | Description |
|-----------|-------|------|-------------|
| `SONAR_TRIG_PIN` | 5 | GPIO | Ultrasonic trigger pin |
| `SONAR_ECHO_PIN` | 18 | GPIO | Ultrasonic echo pin |
| `GREEN_LED_PIN` | 19 | GPIO | Connection OK indicator |
| `RED_LED_PIN` | 21 | GPIO | Connection error indicator |
| `MQTT_BROKER` | test.mosquitto.org | - | MQTT broker hostname |
| `MQTT_PORT` | 1883 | - | MQTT broker port |
| `MQTT_TOPIC` | tank/level | - | MQTT topic for publishing |
| `F` | 1000 | ms | Sampling/publish interval |
| `WIFI_SSID` | (configured) | - | WiFi network name |
| `WIFI_PASS` | (configured) | - | WiFi password |

### 8.3 WCS Configuration (config.h)

| Parameter | Value | Unit | Description |
|-----------|-------|------|-------------|
| `BUTTON_PIN` | 2 | Digital | Mode toggle button |
| `SERVO_PIN` | 9 | PWM | Servo motor control |
| `POT_PIN` | A0 | Analog | Potentiometer input |
| `GREEN_LED_PIN` | 6 | Digital | Connection OK indicator |
| `RED_LED_PIN` | 7 | Digital | Connection error indicator |
| `LCD_ADDR` | 0x27 | I2C | LCD I2C address |
| `LCD_COLS` | 16 | chars | LCD column count |
| `LCD_ROWS` | 2 | rows | LCD row count |

---

## 9. Implementation Details

### 9.1 Project Structure

```
Assignment_3/
├── platformio.ini                    # Multi-target build config
├── src/
│   ├── tms/                          # ESP32 Tank Monitoring
│   │   ├── config.h                  # WiFi, MQTT, pin config
│   │   ├── main.cpp                  # Entry point
│   │   ├── core.cpp/h                # Initialization
│   │   ├── Kernel/
│   │   │   ├── Scheduler.cpp/h       # Task scheduler
│   │   │   └── kernel.cpp/h          # State utilities
│   │   ├── Devices/
│   │   │   ├── Led.cpp/h             # LED abstraction
│   │   │   └── UltrasonicSensor.cpp/h
│   │   └── Tasks/
│   │       ├── Task.h                # Base class
│   │       ├── TmsTask.cpp/h         # Main FSM logic
│   │       ├── BlinkTask.cpp/h       # LED control
│   │       └── UltrasonicTask.cpp/h  # Sensor reading
│   │
│   └── wcs/                          # Arduino Water Channel
│       ├── config.h                  # Pin and protocol config
│       ├── main.cpp                  # Entry point
│       ├── core.cpp/h                # Initialization
│       ├── Kernel/
│       │   ├── Scheduler.cpp/h       # TimerOne scheduler
│       │   └── kernel.cpp/h          # State utilities
│       ├── Devices/
│       │   ├── Button.cpp/h          # Debounced button
│       │   ├── LCD.cpp/h             # I2C LCD wrapper
│       │   ├── Potentiometer.cpp/h   # Analog input
│       │   └── ServoMotor.cpp/h      # Servo control
│       └── Tasks/
│           ├── Task.h                # Base class
│           ├── WcsTask.cpp/h         # Main FSM (3 states)
│           ├── ButtonTask.cpp/h      # Button polling
│           ├── LCDTask.cpp/h         # Display updates
│           └── ServoTask.cpp/h       # Servo positioning
│
├── cus/                              # Java Control Unit
│   ├── pom.xml                       # Maven build
│   └── src/main/java/cus/
│       ├── ControlUnit.java          # Main entry
│       ├── config/Config.java        # All parameters
│       ├── model/
│       │   ├── SystemState.java      # Shared state
│       │   └── SystemMode.java       # Mode enum
│       ├── mqtt/MqttHandler.java     # MQTT subscriber
│       ├── serial/
│       │   ├── SerialHandler.java    # Serial I/O
│       │   └── SerialProtocol.java   # Command constants
│       ├── http/HttpServer.java      # REST API
│       └── policy/WaterLevelPolicy.java  # Control logic
│
└── dbs/                              # Web Dashboard
    ├── public/
    │   ├── index.html                # Main page
    │   ├── dashboard.js              # Chart + controls
    │   ├── app.js                    # API client
    │   ├── config.js                 # Backend URL
    │   └── style.css                 # Styling
    └── README.md                     # Setup instructions
```

### 9.2 Task Base Class

Both TMS and WCS use the same task abstraction:

```cpp
class Task {
  int myPeriod;      // Execution period (ms)
  int timeElapsed;   // Accumulator
  bool active;       // Enabled flag

public:
  virtual void init(int period);
  virtual void tick() = 0;  // Override with FSM logic
  bool updateAndCheckTime(int basePeriod);
  bool isActive();
  void setActive(bool active);
};
```

### 9.3 WCS State Machine Implementation

```cpp
void WcsTask::tick() {
    handleSerialInput();  // Process CUS commands

    // Button toggles mode (except in UNCONNECTED)
    if (button->isButtonPressed()) {
        if (state == MANUAL) {
            state = AUTOMATIC;
            Serial.println(CMD_MODE_AUTO);
        } else if (state == AUTOMATIC) {
            state = MANUAL;
            Serial.println(CMD_MODE_MANUAL);
        }
        updateLcd();
    }

    switch (state) {
        case UNCONNECTED:
            setValveLevel(0);
            break;

        case AUTOMATIC:
            // Valve controlled by CUS commands
            break;

        case MANUAL:
            int potPercent = readPotPercent();
            setValveLevel(potPercent);
            Serial.print(CMD_VALVE_OPEN);
            Serial.println(potPercent);
            break;
    }
}
```

### 9.4 CUS Policy Implementation

```java
private void evaluateAutomatic() {
    double level = state.getLatestWaterLevel();
    int targetValve;

    if (level > Config.L2) {
        targetValve = 100;
        state.resetL1Timer();
    } else if (level > Config.L1) {
        state.startL1Timer();
        if (state.getL1ExceededDuration() >= Config.T1) {
            targetValve = 50;
        } else {
            return;  // Still waiting
        }
    } else {
        targetValve = 0;
        state.resetL1Timer();
    }

    if (targetValve != lastSentValvePercent) {
        state.setValveOpenPercent(targetValve);
        serial.sendValveOpen(targetValve);
        lastSentValvePercent = targetValve;
    }
}
```

---

## 10. Breadboard Schema

### 10.1 Simulation Links

| Subsystem | Platform | Simulation |
|-----------|----------|------------|
| **WCS** | Arduino UNO | [TinkerCad](https://www.tinkercad.com/things/iggcpeCcoL2-assignment-3/editel) |
| **TMS** | ESP32 | [Wokwi](https://wokwi.com/projects/454858190991922177) |

### 10.2 Wiring Summary

**TMS (ESP32):**
```
Component          ESP32 Pin
─────────────────────────────
Sonar Trig    ──── GPIO 5
Sonar Echo    ──── GPIO 18
Green LED (+) ──── GPIO 19
Red LED (+)   ──── GPIO 21
All GND       ──── GND
Sonar VCC     ──── 3.3V
```

**WCS (Arduino UNO):**
```
Component          Arduino Pin
──────────────────────────────
Servo Signal  ──── D9
Button        ──── D2 (pull-up)
Green LED (+) ──── D6
Red LED (+)   ──── D7
Pot Wiper     ──── A0
LCD SDA       ──── A4
LCD SCL       ──── A5
Servo VCC     ──── 5V
LCD VCC       ──── 5V
All GND       ──── GND
```

---

## 11. Demo Video

**Video demonstration**: [Link to video on UNIBO OneDrive]

<!-- TODO: Add link to demonstration video -->

The video demonstrates:
1. System startup with all subsystems connecting
2. TMS publishing water level readings via MQTT
3. CUS receiving data and applying policy rules
4. Automatic valve control when level exceeds L1 (after T1)
5. Immediate 100% valve opening when level exceeds L2
6. Manual mode activation via WCS button
7. Potentiometer control in manual mode
8. Dashboard displaying real-time chart and controls
9. Remote mode switching and valve control from dashboard
10. UNCONNECTED state when MQTT data stops (after T2)
11. Recovery when MQTT data resumes

---

## Appendix A: Build Instructions

### A.1 TMS (ESP32)

```bash
cd Assignment_3

# Configure WiFi credentials in src/tms/config.h
# WIFI_SSID and WIFI_PASS

# Build
pio run -e esp32dev

# Upload
pio run -e esp32dev --target upload

# Monitor (115200 baud)
pio device monitor -e esp32dev
```

### A.2 WCS (Arduino UNO)

```bash
cd Assignment_3

# Build
pio run -e uno

# Upload
pio run -e uno --target upload

# Monitor (9600 baud)
pio device monitor -e uno
```

### A.3 CUS (Java)

```bash
cd Assignment_3/cus

# Build
mvn clean package

# Run
java -jar target/cus-1.0.jar

# Or with Maven
mvn exec:java -Dexec.mainClass="cus.ControlUnit"
```

### A.4 DBS (Web Dashboard)

```bash
cd Assignment_3/dbs/public

# Option 1: Python simple server
python -m http.server 3000

# Option 2: Node.js http-server
npx http-server -p 3000

# Then open: http://localhost:3000
```

---

## Appendix B: Dependencies

### B.1 PlatformIO Libraries

**Arduino UNO (WCS):**
| Library | Version | Purpose |
|---------|---------|---------|
| TimerOne | ^1.2 | Hardware timer for scheduler |
| LiquidCrystal_I2C | ^1.1.4 | I2C LCD driver |

**ESP32 (TMS):**
| Library | Version | Purpose |
|---------|---------|---------|
| PubSubClient | ^2.8 | MQTT client |

### B.2 Java Dependencies (Maven)

| Dependency | Purpose |
|------------|---------|
| Eclipse Paho | MQTT client |
| jSerialComm | Serial port communication |
| Gson | JSON parsing |

### B.3 Web Dependencies

| Library | Purpose |
|---------|---------|
| Chart.js | Water level graph |

---

## Appendix C: Troubleshooting

### C.1 TMS Not Connecting to WiFi

- Verify SSID and password in `config.h`
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)
- Check serial monitor for connection status

### C.2 CUS Not Receiving MQTT Data

- Verify broker URL and topic match between TMS and CUS
- Test broker connectivity: `mosquitto_sub -h test.mosquitto.org -t tank/level`
- Check firewall allows port 1883

### C.3 WCS Not Responding to Commands

- Verify correct COM port in CUS `Config.java`
- Ensure baud rate matches (9600)
- Check serial cable connection

### C.4 Dashboard Shows "NOT AVAILABLE"

- Ensure CUS is running and HTTP server started
- Verify `config.js` has correct backend URL
- Check browser console for CORS errors

---