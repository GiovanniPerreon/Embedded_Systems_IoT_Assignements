# Assignment 3 — Work Distribution

## Smart Tank Monitoring System

Three-person team split. Each person owns one or two subsystems with clear boundaries and integration contracts.

---

## Person 1: TMS (Tank Monitoring Subsystem) + DBS (Dashboard Subsystem)

### TMS — ESP32 Embedded Firmware

**Language**: C/C++ with Arduino framework
**Build tool**: PlatformIO
**Board**: ESP32 (or ESP8266)
**Libraries**: WiFi.h, PubSubClient (MQTT client), NewPing or similar (sonar)

#### Hardware to wire

- 1 ultrasonic sonar sensor (HC-SR04 or equivalent) — measures water level
- 1 green LED — indicates system healthy / data being sent
- 1 red LED — indicates network failure

#### What to implement

- WiFi connection setup and reconnection logic
- MQTT client that connects to a broker (e.g. Mosquitto running on the PC)
- Sonar reading at a configurable sampling frequency **F**
- Publish each water level reading to a defined MQTT topic (e.g. `tank/water-level`)
- FSM with at minimum two states:
  - **CONNECTED**: green LED on, red LED off, readings published normally
  - **DISCONNECTED**: red LED on, green LED off, attempting reconnection
- The control logic must use FSMs and optionally RTOS tasks (FreeRTOS comes built into ESP32)

#### MQTT protocol to define (agree with Person 3)

- Broker address and port (e.g. `localhost:1883`)
- Topic name for water level data (e.g. `tank/water-level`)
- Payload format (e.g. JSON: `{"level": 42.5, "timestamp": 1234567890}`)
- QoS level (0, 1, or 2)
- Frequency of publishing

#### Files to produce (inside `tms/` directory)

- `platformio.ini` — ESP32 board config + library dependencies
- `src/main.cpp` — entry point
- Source files for FSM, WiFi management, MQTT publishing, sonar reading, LED control
- `config.h` — pin assignments, WiFi credentials, broker address, topic names, frequency F

---

### DBS — Dashboard Web Frontend

**Languages**: HTML, CSS, JavaScript
**No backend framework needed** — this is a static frontend that calls CUS via HTTP
**Libraries (suggestions)**: Chart.js or similar for the graph, vanilla JS or a lightweight framework

#### What to implement

- An HTTP client that polls or fetches data from the CUS HTTP API
- A **line graph** showing the last **N** water level measurements over time
- A **text/numeric display** showing the current valve opening percentage (0%–100%)
- A **status indicator** showing the current system state:
  - `AUTOMATIC` — system is controlling the valve automatically
  - `MANUAL` — operator is controlling the valve
  - `UNCONNECTED` — CUS lost contact with TMS
  - `NOT AVAILABLE` — DBS cannot reach CUS at all (HTTP request fails)
- A **button** to toggle between AUTOMATIC and MANUAL mode (sends a request to CUS)
- A **slider or input widget** to set the valve opening percentage when in MANUAL mode (sends the value to CUS)

#### HTTP API to agree on with Person 3

Person 3 defines the endpoints, Person 1 consumes them:

- `GET /api/state` — returns current system state, valve %, recent water level readings
- `POST /api/mode` — toggle or set AUTOMATIC/MANUAL
- `POST /api/valve` — set valve opening % (only works in MANUAL mode)
- Response format (JSON structure, status codes, error handling)

#### Files to produce (inside `dbs/` directory)

- `index.html` — main page
- `style.css` — styling
- `app.js` — HTTP polling, graph rendering, control handlers
- Any additional JS modules as needed

---

## Person 2: WCS (Water Channel Subsystem)

**Language**: C/C++ with Arduino framework
**Build tool**: PlatformIO
**Board**: Arduino UNO
**Libraries**: Servo.h, LiquidCrystal_I2C

### Hardware to wire

- 1 servo motor — controls the water channel valve (0° = closed, 90° = fully open)
- 1 potentiometer — manual valve control input (analog read, mapped to 0%–100%)
- 1 tactile button — toggles between AUTOMATIC and MANUAL mode
- 1 LCD display (I2C, 16x2) — shows valve % and current mode

### What to implement

- **Task-based scheduler** — reuse the Scheduler and Task pattern from Assignment 2
- **Serial communication** with CUS:
  - Receive commands: valve opening percentage, mode changes
  - Send responses: current state acknowledgments
- **Button handling** with debounce:
  - Press toggles AUTOMATIC ↔ MANUAL mode
  - Notify CUS of mode change via serial
- **Potentiometer reading**:
  - Only active in MANUAL mode
  - Analog read mapped from 0–1023 to 0%–100%
  - Controls servo position directly
- **Servo control**:
  - In AUTOMATIC mode: position set by commands from CUS
  - In MANUAL mode: position set by potentiometer
  - Map 0%–100% to 0°–90°
- **LCD display** — update periodically with:
  - Line 1: valve opening level (e.g. `Valve: 50%`)
  - Line 2: current mode (`AUTOMATIC`, `MANUAL`, or `UNCONNECTED`)
- **FSM** with states such as:
  - **AUTOMATIC**: servo controlled by CUS commands, potentiometer ignored
  - **MANUAL**: servo controlled by potentiometer, CUS commands ignored (but still report state)
  - **UNCONNECTED**: displayed when CUS signals loss of TMS connection

### Serial protocol to define (agree with Person 3)

- Baud rate (e.g. 9600, consistent with previous assignments)
- Command format CUS → WCS (e.g. `SET_VALVE:50`, `SET_MODE:MANUAL`, `SET_MODE:AUTO`)
- Response format WCS → CUS (e.g. `STATE:MANUAL:75` meaning mode=MANUAL, valve=75%)
- How often WCS reports its state back
- How mode changes initiated by the physical button are communicated to CUS

### Files to produce (inside `wcs/` directory)

- `platformio.ini` — Arduino UNO board config + library dependencies
- `src/main.cpp` — entry point
- `src/Kernel/Scheduler.h/.cpp` — reused from Assignment 2
- `src/Kernel/Task.h` — base task class, reused from Assignment 2
- `src/Tasks/` — individual task files: ServoTask, ButtonTask, LCDTask, PotentiometerTask, SerialTask
- `src/Devices/` — hardware abstraction for each component
- `src/config.h` — pin assignments, serial baud rate, protocol constants

---

## Person 3: CUS (Control Unit Subsystem)

**Language**: Java
**Build tool**: Maven or Gradle
**Libraries**: Eclipse Paho (MQTT client), jSerialComm or RXTX (serial), a lightweight HTTP server (Javalin, Spark, or Java's built-in `com.sun.net.httpserver`)

### What to implement

#### MQTT Communication (with TMS)

- Connect to the MQTT broker
- Subscribe to the water level topic defined by Person 1
- Parse incoming water level readings
- Store the last **N** readings in memory for serving to DBS
- Track time since last message received — if exceeding **T2**, enter UNCONNECTED state

#### Serial Communication (with WCS)

- Open serial port connection to Arduino
- Send valve opening commands using the protocol defined by Person 2
- Receive state updates (current mode, valve %) from WCS
- Forward mode change requests (from DBS or from WCS button presses)

#### HTTP Server (for DBS)

- Expose REST API endpoints:
  - `GET /api/state` — return JSON with: system mode, valve %, last N water level readings, current state
  - `POST /api/mode` — accept mode change requests (AUTOMATIC/MANUAL) from dashboard
  - `POST /api/valve` — accept valve opening % (only in MANUAL mode)
- Handle CORS headers if DBS is served from a different origin

#### Policy Logic (the core brain)

- Maintain global system state: `AUTOMATIC`, `MANUAL`, `UNCONNECTED`
- In AUTOMATIC mode, apply the water level policy:
  - Level > **L1** (but < L2) sustained for **T1** time → send `50%` valve open command to WCS
  - Level > **L2** → **immediately** send `100%` valve open command to WCS
  - Level drops below thresholds → close valve (send `0%`)
- In MANUAL mode:
  - Accept valve % from DBS (via HTTP) or from WCS (via potentiometer/serial)
  - Forward the value to WCS
- UNCONNECTED state:
  - Triggered when no MQTT data received for **T2** time
  - Notify WCS to display UNCONNECTED on LCD
  - Restored automatically when data resumes

#### Configuration

All parameters in a config file or constants class:

- **L1**, **L2**: water level thresholds
- **T1**: time before L1 triggers action
- **T2**: timeout for UNCONNECTED state
- **N**: number of readings to store for the graph
- MQTT broker address/port/topic
- Serial port name and baud rate
- HTTP server port

### Files to produce (inside `cus/` directory)

- `pom.xml` or `build.gradle` — dependencies
- `src/main/java/cus/ControlUnit.java` — entry point, wires everything together
- `src/main/java/cus/mqtt/MqttHandler.java` — MQTT subscription and parsing
- `src/main/java/cus/serial/SerialHandler.java` — serial communication with WCS
- `src/main/java/cus/http/HttpServer.java` — REST API endpoints
- `src/main/java/cus/model/SystemState.java` — state management, mode tracking, water level history
- `src/main/java/cus/policy/WaterLevelPolicy.java` — L1/L2/T1/T2 threshold logic
- `src/main/java/cus/config/Config.java` — all configurable parameters

---

## Shared Responsibilities (all three people)

- **Documentation** (`doc/` directory): each person documents their own subsystem's FSM diagrams in the final report
- **Integration testing**: once individual subsystems work, all three meet to test end-to-end
- **Demo video**: record together showing the full system working
- **Protocol agreements**: lock down MQTT format (Person 1 + 3) and serial format (Person 2 + 3) before starting implementation

## Integration Boundaries Summary

| Boundary | Between | Protocol | Who defines it |
|----------|---------|----------|----------------|
| TMS ↔ CUS | Person 1 ↔ Person 3 | MQTT | Person 1 defines, Person 3 consumes |
| WCS ↔ CUS | Person 2 ↔ Person 3 | Serial | Person 2 defines, Person 3 consumes |
| CUS ↔ DBS | Person 3 ↔ Person 1 | HTTP | Person 3 defines, Person 1 consumes |
