package dru.ui;

import dru.model.DroneRemoteModel;
import dru.model.DroneState;
import dru.model.HangarState;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.util.Duration;

/**
 * JavaFX Controller for Drone Remote Unit GUI.
 */
public class DroneRemoteController {

    // Connection controls
    @FXML private ComboBox<String> portComboBox;
    @FXML private Button connectButton;
    @FXML private Button refreshButton;
    @FXML private Label connectionStatusLabel;

    // Command buttons
    @FXML private Button takeoffButton;
    @FXML private Button landButton;

    // State displays
    @FXML private Label droneStateLabel;
    @FXML private Label hangarStateLabel;
    @FXML private Circle droneStateIndicator;
    @FXML private Circle hangarStateIndicator;

    // Landing distance display
    @FXML private VBox distanceBox;
    @FXML private Label distanceLabel;
    @FXML private ProgressBar distanceBar;

    // Debug/log area
    @FXML private TextArea logArea;

    private DroneRemoteModel model;
    private Timeline statusPollingTimeline;

    @FXML
    public void initialize() {
        model = new DroneRemoteModel();
        setupBindings();
        setupListeners();
        refreshPorts();
        setupStatusPolling();
    }

    private void setupBindings() {
        // Connection status
        connectionStatusLabel.textProperty().bind(model.statusMessageProperty());

        // Button enable states based on connection and drone state
        takeoffButton.disableProperty().bind(
            model.connectedProperty().not()
                .or(model.droneStateProperty().isNotEqualTo(DroneState.DRONE_REST))
                .or(model.hangarStateProperty().isEqualTo(HangarState.ALARM))
        );

        landButton.disableProperty().bind(
            model.connectedProperty().not()
                .or(model.droneStateProperty().isNotEqualTo(DroneState.DRONE_OPERATING))
                .or(model.hangarStateProperty().isEqualTo(HangarState.ALARM))
        );

        // Distance display visibility
        distanceBox.visibleProperty().bind(
            model.droneStateProperty().isEqualTo(DroneState.DRONE_LANDING)
        );
    }

    private void setupListeners() {
        // Drone state changes
        model.droneStateProperty().addListener((obs, oldState, newState) -> {
            updateDroneStateDisplay(newState);
            log("Drone state: " + newState.getDisplayName());
        });

        // Hangar state changes
        model.hangarStateProperty().addListener((obs, oldState, newState) -> {
            updateHangarStateDisplay(newState);
            log("Hangar state: " + newState.getDisplayName());

            if (newState == HangarState.ALARM) {
                showAlarmAlert();
            }
        });

        // Landing distance updates
        model.landingDistanceProperty().addListener((obs, oldDist, newDist) -> {
            double dist = newDist.doubleValue();
            if (dist >= 0) {
                distanceLabel.setText(String.format("%.1f cm", dist));
                // Normalize distance for progress bar (assuming max 100cm)
                double normalized = Math.max(0, Math.min(1, dist / 100.0));
                distanceBar.setProgress(1.0 - normalized); // Invert so full = close
            }
        });

        // Last message for logging
        model.lastMessageProperty().addListener((obs, oldMsg, newMsg) -> {
            if (!newMsg.isEmpty()) {
                log("RX: " + newMsg);
            }
        });

        // Connection state
        model.connectedProperty().addListener((obs, wasConnected, isConnected) -> {
            connectButton.setText(isConnected ? "Disconnect" : "Connect");
            portComboBox.setDisable(isConnected);
            refreshButton.setDisable(isConnected);
        });
    }

    private void setupStatusPolling() {
        // Poll for status every 2 seconds when connected
        statusPollingTimeline = new Timeline(
            new KeyFrame(Duration.seconds(2), e -> {
                if (model.isConnected()) {
                    model.sendStatusQuery();
                }
            })
        );
        statusPollingTimeline.setCycleCount(Timeline.INDEFINITE);
    }

    private void updateDroneStateDisplay(DroneState state) {
        droneStateLabel.setText(state.getDisplayName());

        Color color = switch (state) {
            case DRONE_REST -> Color.LIGHTGREEN;
            case DRONE_TAKEOFF -> Color.YELLOW;
            case DRONE_OPERATING -> Color.DEEPSKYBLUE;
            case DRONE_LANDING -> Color.ORANGE;
            case ALARM -> Color.RED;
        };
        droneStateIndicator.setFill(color);
    }

    private void updateHangarStateDisplay(HangarState state) {
        hangarStateLabel.setText(state.getDisplayName());

        Color color = switch (state) {
            case NORMAL -> Color.LIGHTGREEN;
            case PREALARM -> Color.YELLOW;
            case ALARM -> Color.RED;
        };
        hangarStateIndicator.setFill(color);
    }

    private void showAlarmAlert() {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle("ALARM");
        alert.setHeaderText("Temperature Alarm!");
        alert.setContentText(
            "The hangar has entered ALARM state due to critical temperature.\n\n" +
            "All operations are suspended.\n" +
            "Press RESET button on the hangar to clear alarm."
        );
        alert.show();
    }

    // FXML Action handlers

    @FXML
    private void onRefreshPorts() {
        refreshPorts();
    }

    private void refreshPorts() {
        portComboBox.getItems().clear();
        String[] ports = model.getAvailablePorts();
        portComboBox.getItems().addAll(ports);
        if (ports.length > 0) {
            portComboBox.getSelectionModel().selectFirst();
        }
    }

    @FXML
    private void onConnect() {
        if (model.isConnected()) {
            model.disconnect();
            statusPollingTimeline.stop();
            log("Disconnected");
        } else {
            String port = portComboBox.getValue();
            if (port == null || port.isEmpty()) {
                showError("No Port Selected", "Please select a COM port.");
                return;
            }

            if (model.connect(port)) {
                log("Connected to " + port);
                statusPollingTimeline.play();
                // Request initial status
                model.sendStatusQuery();
            } else {
                showError("Connection Failed", "Could not connect to " + port);
            }
        }
    }

    @FXML
    private void onTakeoff() {
        if (model.sendTakeoff()) {
            log("TX: TAKEOFF");
        } else {
            log("Failed to send TAKEOFF command");
        }
    }

    @FXML
    private void onLand() {
        if (model.sendLand()) {
            log("TX: LAND");
        } else {
            log("Failed to send LAND command");
        }
    }

    private void log(String message) {
        String timestamp = java.time.LocalTime.now().toString().substring(0, 8);
        logArea.appendText("[" + timestamp + "] " + message + "\n");
    }

    private void showError(String title, String message) {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle(title);
        alert.setContentText(message);
        alert.showAndWait();
    }

    /**
     * Clean up resources when application closes.
     */
    public void shutdown() {
        statusPollingTimeline.stop();
        model.disconnect();
    }
}
