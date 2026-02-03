package cus;

import cus.config.Config;
import cus.http.HttpServer;
import cus.model.SystemMode;
import cus.model.SystemState;
import cus.mqtt.MqttHandler;
import cus.policy.WaterLevelPolicy;
import cus.serial.SerialHandler;
import cus.serial.SerialProtocol;

public class ControlUnit {

    private final SystemState state;
    private final MqttHandler mqtt;
    private final SerialHandler serial;
    private final HttpServer http;
    private final WaterLevelPolicy policy;
    private Thread policyThread;

    public ControlUnit() {
        this.state = new SystemState();
        this.serial = new SerialHandler();
        this.mqtt = new MqttHandler(state);
        this.http = new HttpServer(state, serial);
        this.policy = new WaterLevelPolicy(state, serial);
    }

    public void start(String serialPort) {
        System.out.println("=== CUS - Control Unit Subsystem ===");

        // 1. Setup serial message handler
        serial.setMessageHandler(this::handleSerialMessage);

        // 2. Connect serial to WCS
        System.out.println("Available serial ports: "
            + String.join(", ", SerialHandler.getAvailablePorts()));
        if (!serial.connect(serialPort)) {
            System.err.println("[WARN] Could not connect to WCS on " + serialPort);
        }

        // 3. Connect MQTT
        try {
            if (mqtt.isConnected()) {
                mqtt.disconnect(); // Ensure clean disconnect before reconnect
            }
            mqtt.connect();
        } catch (Exception e) {
            System.err.println("[WARN] Could not connect to MQTT: " + e.getMessage());
        }

        // Start fast MQTT reconnect thread
        Thread mqttReconnectThread = new Thread(() -> {
            while (true) {
                try {
                    if (!mqtt.isConnected()) {
                        System.out.println("[MQTT] Disconnected, attempting reconnect...");
                        try {
                            mqtt.disconnect(); // Ensure clean disconnect before reconnect
                        } catch (Exception ignored) {}
                        mqtt.connect();
                    }
                } catch (Exception e) {
                    System.err.println("[MQTT] Reconnect failed: " + e.getMessage());
                }
                try {
                    Thread.sleep(1000); // Check every 1 second (faster reconnect)
                } catch (InterruptedException ignored) {}
            }
        }, "MqttReconnectThread");
        mqttReconnectThread.setDaemon(true);
        mqttReconnectThread.start();

        // 4. Start HTTP server
        http.start();

        // 5. Start policy thread
        policyThread = new Thread(policy, "PolicyEvalThread");
        policyThread.setDaemon(true);
        policyThread.start();

        System.out.println("=== CUS fully started ===");
    }

    private void handleSerialMessage(String message) {
        switch (message) {
            case SerialProtocol.RESP_MANUAL -> {
                System.out.println("[CUS] WCS -> MANUAL (button press)");
                state.setMode(SystemMode.MANUAL);
            }
            case SerialProtocol.RESP_AUTOMATIC -> {
                System.out.println("[CUS] WCS -> AUTOMATIC (button press)");
                state.setMode(SystemMode.AUTOMATIC);
            }
            case SerialProtocol.RESP_UNCONNECTED -> {
                System.out.println("[CUS] WCS reports UNCONNECTED");
                state.setMode(SystemMode.UNCONNECTED);
            }
            default -> {
                if (message.startsWith(SerialProtocol.RESP_VALVE_OPEN)) {
                    try {
                        int pct = Integer.parseInt(
                            message.substring(SerialProtocol.RESP_VALVE_OPEN.length()).trim());
                        state.setValveOpenPercent(pct);
                        System.out.println("[CUS] WCS valve at " + pct + "%");
                    } catch (NumberFormatException e) {
                        System.err.println("[CUS] Invalid valve response: " + message);
                    }
                } else {
                    System.out.println("[CUS] Unknown serial message: " + message);
                }
            }
        }
    }

    public void stop() {
        System.out.println("Shutting down CUS...");
        policy.stop();
        http.stop();
        serial.disconnect();
        mqtt.disconnect();
        System.out.println("CUS shut down.");
    }

    public static void main(String[] args) {
        ControlUnit cu = new ControlUnit();
        String port = args.length > 0 ? args[0] : Config.SERIAL_PORT;

        Runtime.getRuntime().addShutdownHook(new Thread(cu::stop));

        cu.start(port);
        System.out.println("CUS running. Press Ctrl+C to stop.");
    }
}
