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

## 2. Hardware Components

### TMS (ESP32)

| Component | GPIO | Function |
|-----------|------|----------|
| Ultrasonic Sonar | 5/18 | Water level measurement |
| Green LED | 19 | Network OK |
| Red LED | 21 | Network error |

### WCS (Arduino UNO)

| Component | Pin | Function |
|-----------|-----|----------|
| Servo Motor | D9 | Valve (0°=closed, 90°=open) |
| Button | D2 | Mode toggle (AUTO↔MANUAL) |
| Potentiometer | A0 | Manual valve control |
| Green LED | D6 | Connection OK |
| Red LED | D7 | Connection error |
| LCD I2C 16x2 | A4/A5 | Mode and valve display |

---

## 3. Software Architecture

All embedded subsystems use a **task-based cooperative scheduler**.

| Subsystem | Tasks |
|-----------|-------|
| TMS | TmsTask (MQTT publish), UltrasonicTask, BlinkTask |
| WCS | WcsTask (FSM), ButtonTask, ServoTask, LCDTask |
| CUS | MqttHandler, SerialHandler, HttpServer, WaterLevelPolicy |

---

## 4. Finite State Machines

### 4.1 TMS FSM (Connection Status)

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

### 4.2 WCS FSM (Mode Control - 3 states)

```
                              MODE_AUTO cmd / Button
            ┌─────────────────────────────────────────────┐
            │                                             │
            ▼                                             │
    ┌───────────────┐       MODE_MANUAL / Button    ┌─────┴───────┐
    │   AUTOMATIC   │◄─────────────────────────────►│    MANUAL   │
    │               │                               │             │
    │ Valve by CUS  │                               │ Valve by Pot│
    │ Pot ignored   │                               │ or Dashboard│
    └───────┬───────┘                               └──────┬──────┘
            │                                              │
            │  UNCONNECTED cmd                             │
            │  (no MQTT for T2)                            │
            ▼                                              │
    ┌───────────────┐                                      │
    │  UNCONNECTED  │◄─────────────────────────────────────┘
    │  Valve = 0%   │         UNCONNECTED cmd
    │  Button disabled│
    └───────┬───────┘
            │ MQTT data resumes
            └──────────────────────────────────────────────►
```

| State | Valve Control | Button | Potentiometer |
|-------|---------------|--------|---------------|
| UNCONNECTED | Fixed 0% | Disabled | Ignored |
| AUTOMATIC | CUS policy | Toggle to MANUAL | Ignored |
| MANUAL | Pot or Dashboard | Toggle to AUTO | Active |

### 4.3 CUS Policy FSM (Water Level Control)

```
                          No MQTT for T2
    ┌──────────────────────────────────────────────────────┐
    │                                                      │
    ▼                                                      │
┌───────────────┐                                          │
│  UNCONNECTED  │──── MQTT resumes ────►┐                  │
│  Valve = 0%   │                       │                  │
└───────────────┘                       │                  │
                                        ▼                  │
                    level ≤ L1   ┌──────────────┐          │
         ┌───────────────────────│    NORMAL    │          │
         │                       │  Valve = 0%  │          │
         │                       └──────┬───────┘          │
         │                              │ level > L1       │
         │                              ▼                  │
         │                       ┌──────────────┐          │
         │    level ≤ L1         │     HIGH     │          │
         │◄──────────────────────│  Timer T1    │          │
         │                       └──────┬───────┘          │
         │                              │ T1 elapsed       │
         │                              ▼                  │
         │                       ┌──────────────┐          │
         │    level ≤ L1         │   WARNING    │          │
         │◄──────────────────────│  Valve = 50% │          │
         │                       └──────┬───────┘          │
         │                              │ level > L2       │
         │                              ▼                  │
         │                       ┌──────────────┐          │
         │    level ≤ L1         │   CRITICAL   │──────────┘
         │◄──────────────────────│ Valve = 100% │  no MQTT
         │                       └──────────────┘
         ▼
    [Back to NORMAL]
```

| State | Water Level | Valve |
|-------|-------------|-------|
| UNCONNECTED | Unknown | 0% |
| NORMAL | ≤ L1 | 0% |
| HIGH | > L1, ≤ L2 | Unchanged (timer running) |
| WARNING | > L1 for T1 | 50% |
| CRITICAL | > L2 | 100% |

---

## 5. Communication Protocols

### MQTT (TMS → CUS)

| Parameter | Value |
|-----------|-------|
| Broker | test.mosquitto.org:1883 |
| Topic | `tank/level` |
| Payload | `{"level": 42.5}` |

### Serial (CUS ↔ WCS) - 9600 baud

| Command | Description |
|---------|-------------|
| `MODE_MANUAL` / `MODE_AUTO` | Set mode |
| `VALVE_OPEN:<n>` | Set valve to n% |
| `STATUS?` | Query state |
| `UNCONNECTED` | Enter unconnected state |

### HTTP REST (CUS ↔ DBS) - Port 8080

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/state` | GET | Get system state + history |
| `/api/mode` | POST | Set mode |
| `/api/valve` | POST | Set valve % |

---

## 6. Configuration Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| L1 | 30 cm | First threshold (50% valve) |
| L2 | 50 cm | Second threshold (100% valve) |
| T1 | 5000 ms | Time before L1 triggers |
| T2 | 10000 ms | Timeout for UNCONNECTED |
| N | 50 | Dashboard history size |
| F | 1000 ms | TMS publish interval |

---

## 7. Breadboard Schema

| Subsystem | Simulation |
|-----------|------------|
| WCS (Arduino) | [TinkerCad](https://www.tinkercad.com/things/iggcpeCcoL2-assignment-3/editel) |
| TMS (ESP32) | [Wokwi](https://wokwi.com/projects/454858190991922177) |

---

## 8. Build & Run

**TMS (ESP32):**
```bash
pio run -e esp32dev --target upload && pio device monitor -e esp32dev
```

**WCS (Arduino):**
```bash
pio run -e uno --target upload && pio device monitor -e uno
```

**CUS (Java):**
```bash
cd cus && mvn exec:java -Dexec.mainClass="cus.ControlUnit"
```

**DBS (Dashboard):**
```bash
cd dbs/public && python -m http.server 3000
```

---

*Embedded Systems and IoT - ISI LT - a.y. 2025/2026*
