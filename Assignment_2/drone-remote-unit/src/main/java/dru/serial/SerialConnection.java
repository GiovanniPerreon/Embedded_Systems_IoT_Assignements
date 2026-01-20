package dru.serial;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

import java.util.function.Consumer;

/**
 * Manages serial port connection to Arduino Drone Hangar.
 */
public class SerialConnection {

    private SerialPort serialPort;
    private Consumer<String> messageHandler;
    private Consumer<Boolean> connectionHandler;
    private StringBuilder messageBuffer = new StringBuilder();
    private volatile boolean connected = false;

    /**
     * Get list of available serial ports.
     */
    public static String[] getAvailablePorts() {
        SerialPort[] ports = SerialPort.getCommPorts();
        String[] portNames = new String[ports.length];
        for (int i = 0; i < ports.length; i++) {
            portNames[i] = ports[i].getSystemPortName();
        }
        return portNames;
    }

    /**
     * Set handler for incoming messages from Arduino.
     */
    public void setMessageHandler(Consumer<String> handler) {
        this.messageHandler = handler;
    }

    /**
     * Set handler for connection state changes.
     */
    public void setConnectionHandler(Consumer<Boolean> handler) {
        this.connectionHandler = handler;
    }

    /**
     * Connect to specified serial port.
     * @param portName COM port name (e.g., "COM3" on Windows)
     * @return true if connection successful
     */
    public boolean connect(String portName) {
        if (connected) {
            disconnect();
        }

        serialPort = SerialPort.getCommPort(portName);
        serialPort.setBaudRate(SerialProtocol.BAUD_RATE);
        serialPort.setNumDataBits(SerialProtocol.DATA_BITS);
        serialPort.setNumStopBits(SerialProtocol.STOP_BITS);
        serialPort.setParity(SerialProtocol.PARITY);
        serialPort.setComPortTimeouts(
            SerialPort.TIMEOUT_READ_SEMI_BLOCKING,
            SerialProtocol.READ_TIMEOUT_MS,
            SerialProtocol.WRITE_TIMEOUT_MS
        );

        if (!serialPort.openPort()) {
            return false;
        }

        // Add data listener for incoming messages
        serialPort.addDataListener(new SerialPortDataListener() {
            @Override
            public int getListeningEvents() {
                return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
            }

            @Override
            public void serialEvent(SerialPortEvent event) {
                if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) {
                    return;
                }

                int available = serialPort.bytesAvailable();
                if (available <= 0) return;

                byte[] buffer = new byte[available];
                serialPort.readBytes(buffer, available);

                String data = new String(buffer);
                processIncomingData(data);
            }
        });

        connected = true;
        if (connectionHandler != null) {
            connectionHandler.accept(true);
        }
        return true;
    }

    /**
     * Disconnect from serial port.
     */
    public void disconnect() {
        if (serialPort != null && serialPort.isOpen()) {
            serialPort.removeDataListener();
            serialPort.closePort();
        }
        connected = false;
        if (connectionHandler != null) {
            connectionHandler.accept(false);
        }
    }

    /**
     * Check if connected to serial port.
     */
    public boolean isConnected() {
        return connected && serialPort != null && serialPort.isOpen();
    }

    /**
     * Send command to Arduino.
     * @param command Command string to send
     * @return true if sent successfully
     */
    public boolean send(String command) {
        if (!isConnected()) {
            return false;
        }

        String message = command + "\n";
        byte[] bytes = message.getBytes();
        int written = serialPort.writeBytes(bytes, bytes.length);
        return written == bytes.length;
    }

    /**
     * Send TAKEOFF command.
     */
    public boolean sendTakeoff() {
        return send(SerialProtocol.CMD_TAKEOFF);
    }

    /**
     * Send LAND command.
     */
    public boolean sendLand() {
        return send(SerialProtocol.CMD_LAND);
    }

    /**
     * Send STATUS? query.
     */
    public boolean sendStatusQuery() {
        return send(SerialProtocol.CMD_STATUS);
    }

    /**
     * Process incoming serial data and extract complete messages.
     */
    private void processIncomingData(String data) {
        messageBuffer.append(data);

        int newlineIndex;
        while ((newlineIndex = messageBuffer.indexOf("\n")) != -1) {
            String message = messageBuffer.substring(0, newlineIndex).trim();
            messageBuffer.delete(0, newlineIndex + 1);

            if (!message.isEmpty() && messageHandler != null) {
                messageHandler.accept(message);
            }
        }
    }

    /**
     * Get current port name.
     */
    public String getPortName() {
        return serialPort != null ? serialPort.getSystemPortName() : null;
    }
}
