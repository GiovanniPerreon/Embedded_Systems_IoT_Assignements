# Drone Remote Unit (DRU)

JavaFX application for controlling the Smart Drone Hangar system.

## Requirements

- Java 17 or later
- Maven 3.6+

## Build

```bash
cd drone-remote-unit
mvn clean package
```

## Run

```bash
mvn javafx:run
```

Or after building:
```bash
java --module-path target/classes --add-modules javafx.controls,javafx.fxml -jar target/drone-remote-unit-1.0.0.jar
```

## Features

- **Serial Connection**: Connect to Arduino via COM port
- **Command Buttons**: TAKE OFF and LAND buttons (enabled based on drone state)
- **State Display**: Visual indicators for drone and hangar state
- **Landing Distance**: Real-time distance display during landing
- **Alarm Handling**: Alert dialog and button disable on alarm state
- **Communication Log**: View all serial communication

## Serial Protocol

### Commands (PC -> Arduino)
| Command | Description |
|---------|-------------|
| `TAKEOFF` | Request hangar door open for takeoff |
| `LAND` | Request hangar door open for landing |
| `STATUS?` | Request current status |

### Responses (Arduino -> PC)
| Response | Description |
|----------|-------------|
| `INSIDE` | Drone is inside hangar at rest |
| `OUTSIDE` | Drone is outside operating |
| `TAKEOFF` | Drone is taking off |
| `LANDING` | Drone is landing |
| `ALARM` | System is in alarm state |
| `DIST:<value>` | Landing distance in cm |

## DRU State Machine

```
DRONE_REST --> DRONE_TAKEOFF --> DRONE_OPERATING --> DRONE_LANDING --> DRONE_REST
     |              |                  |                  |
     +------>-------+---------->-------+---------->-------+----> ALARM
```

## Project Structure

```
drone-remote-unit/
├── pom.xml                           # Maven build configuration
├── README.md                         # This file
└── src/main/
    ├── java/
    │   ├── module-info.java          # Java module definition
    │   └── dru/
    │       ├── DroneRemoteUnit.java  # Main application entry
    │       ├── model/
    │       │   ├── DroneState.java   # Drone state enum
    │       │   ├── HangarState.java  # Hangar state enum
    │       │   └── DroneRemoteModel.java # Application model
    │       ├── serial/
    │       │   ├── SerialProtocol.java   # Protocol constants
    │       │   └── SerialConnection.java # Serial port handler
    │       └── ui/
    │           └── DroneRemoteController.java # GUI controller
    └── resources/dru/ui/
        ├── DroneRemoteView.fxml      # GUI layout
        └── style.css                 # Styling
```

## Coordination with Arduino (Person B)

The Arduino code needs to implement the serial protocol:

```cpp
// In loop or serial task:
if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "TAKEOFF") {
        // Handle takeoff request
    } else if (cmd == "LAND") {
        // Handle landing request
    } else if (cmd == "STATUS?") {
        // Send current state
        Serial.println(currentState); // INSIDE, OUTSIDE, TAKEOFF, LANDING, or ALARM
    }
}

// During landing, send distance:
Serial.println("DIST:" + String(distance));

// On alarm:
Serial.println("ALARM");
```
