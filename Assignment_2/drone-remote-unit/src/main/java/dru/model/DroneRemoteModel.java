package dru.model;

import dru.serial.SerialConnection;
import dru.serial.SerialProtocol;
import javafx.application.Platform;
import javafx.beans.property.*;

/**
 * Model for Drone Remote Unit.
 * Manages drone state, hangar state, and serial communication.
 */
public class DroneRemoteModel {

    private final SerialConnection serialConnection;

    // Observable properties for JavaFX binding
    private final ObjectProperty<DroneState> droneState = new SimpleObjectProperty<>(DroneState.DRONE_REST);
    private final ObjectProperty<HangarState> hangarState = new SimpleObjectProperty<>(HangarState.NORMAL);
    private final DoubleProperty landingDistance = new SimpleDoubleProperty(-1);
    private final BooleanProperty connected = new SimpleBooleanProperty(false);
    private final StringProperty lastMessage = new SimpleStringProperty("");
    private final StringProperty statusMessage = new SimpleStringProperty("Disconnected");

    public DroneRemoteModel() {
        this.serialConnection = new SerialConnection();
        setupSerialHandlers();
    }

    private void setupSerialHandlers() {
        serialConnection.setMessageHandler(this::handleMessage);
        serialConnection.setConnectionHandler(isConnected -> {
            Platform.runLater(() -> {
                connected.set(isConnected);
                if (isConnected) {
                    statusMessage.set("Connected to " + serialConnection.getPortName());
                } else {
                    statusMessage.set("Disconnected");
                }
            });
        });
    }

    /**
     * Handle incoming message from Arduino.
     */
    private void handleMessage(String message) {
        Platform.runLater(() -> {
            lastMessage.set(message);
            parseAndUpdateState(message);
        });
    }

    /**
     * Parse message and update state accordingly.
     */
    private void parseAndUpdateState(String message) {
        // Check for distance reading
        if (message.startsWith(SerialProtocol.RESP_DIST_PREFIX)) {
            try {
                String distStr = message.substring(SerialProtocol.RESP_DIST_PREFIX.length());
                double dist = Double.parseDouble(distStr.trim());
                landingDistance.set(dist);
            } catch (NumberFormatException e) {
                // Invalid distance format, ignore
            }
            return;
        }

        // Check for state messages
        switch (message) {
            case SerialProtocol.RESP_INSIDE:
                droneState.set(DroneState.DRONE_REST);
                hangarState.set(HangarState.NORMAL);
                landingDistance.set(-1);
                break;

            case SerialProtocol.RESP_OUTSIDE:
                droneState.set(DroneState.DRONE_OPERATING);
                hangarState.set(HangarState.NORMAL);
                landingDistance.set(-1);
                break;

            case SerialProtocol.RESP_TAKEOFF:
                droneState.set(DroneState.DRONE_TAKEOFF);
                break;

            case SerialProtocol.RESP_LANDING:
                droneState.set(DroneState.DRONE_LANDING);
                break;

            case SerialProtocol.RESP_ALARM:
                droneState.set(DroneState.ALARM);
                hangarState.set(HangarState.ALARM);
                break;

            default:
                // Unknown message, could log for debugging
                break;
        }
    }

    // Connection methods

    public String[] getAvailablePorts() {
        return SerialConnection.getAvailablePorts();
    }

    public boolean connect(String portName) {
        return serialConnection.connect(portName);
    }

    public void disconnect() {
        serialConnection.disconnect();
    }

    public boolean isConnected() {
        return serialConnection.isConnected();
    }

    // Command methods

    public boolean sendTakeoff() {
        if (!canTakeoff()) {
            return false;
        }
        return serialConnection.sendTakeoff();
    }

    public boolean sendLand() {
        if (!canLand()) {
            return false;
        }
        return serialConnection.sendLand();
    }

    public boolean sendStatusQuery() {
        return serialConnection.sendStatusQuery();
    }

    // State query methods

    public boolean canTakeoff() {
        return isConnected() && droneState.get().canTakeoff() && !hangarState.get().isAlarm();
    }

    public boolean canLand() {
        return isConnected() && droneState.get().canLand() && !hangarState.get().isAlarm();
    }

    // Property getters for JavaFX binding

    public ObjectProperty<DroneState> droneStateProperty() {
        return droneState;
    }

    public DroneState getDroneState() {
        return droneState.get();
    }

    public ObjectProperty<HangarState> hangarStateProperty() {
        return hangarState;
    }

    public HangarState getHangarState() {
        return hangarState.get();
    }

    public DoubleProperty landingDistanceProperty() {
        return landingDistance;
    }

    public double getLandingDistance() {
        return landingDistance.get();
    }

    public BooleanProperty connectedProperty() {
        return connected;
    }

    public StringProperty lastMessageProperty() {
        return lastMessage;
    }

    public StringProperty statusMessageProperty() {
        return statusMessage;
    }
}
