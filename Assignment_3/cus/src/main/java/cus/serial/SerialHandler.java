package cus.serial;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;
import cus.config.Config;

import java.util.function.Consumer;

public class SerialHandler {

    private SerialPort serialPort;
    private Consumer<String> messageHandler;
    private final StringBuilder messageBuffer = new StringBuilder();
    private volatile boolean connected = false;

    public void setMessageHandler(Consumer<String> handler) {
        this.messageHandler = handler;
    }

    public boolean connect() {
        return connect(Config.SERIAL_PORT);
    }

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
            System.err.println("[Serial] Failed to open port: " + portName);
            return false;
        }

        serialPort.addDataListener(new SerialPortDataListener() {
            @Override
            public int getListeningEvents() {
                return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
            }

            @Override
            public void serialEvent(SerialPortEvent event) {
                if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) return;
                int available = serialPort.bytesAvailable();
                if (available <= 0) return;
                byte[] buffer = new byte[available];
                serialPort.readBytes(buffer, available);
                processIncomingData(new String(buffer));
            }
        });

        connected = true;
        System.out.println("[Serial] Connected to " + portName);
        return true;
    }

    public void disconnect() {
        if (serialPort != null && serialPort.isOpen()) {
            serialPort.removeDataListener();
            serialPort.closePort();
        }
        connected = false;
        System.out.println("[Serial] Disconnected");
    }

    public boolean isConnected() {
        return connected && serialPort != null && serialPort.isOpen();
    }

    public synchronized boolean send(String command) {
        if (!isConnected()) return false;
        String message = command + "\n";
        byte[] bytes = message.getBytes();
        int written = serialPort.writeBytes(bytes, bytes.length);
        System.out.println("[Serial] TX: " + command);
        return written == bytes.length;
    }

    public boolean sendModeManual() {
        return send(SerialProtocol.CMD_MODE_MANUAL);
    }

    public boolean sendModeAuto() {
        return send(SerialProtocol.CMD_MODE_AUTO);
    }

    public boolean sendValveOpen(int percent) {
        return send(SerialProtocol.CMD_VALVE_OPEN + percent);
    }

    public boolean sendStatusQuery() {
        return send(SerialProtocol.CMD_STATUS);
    }

    public boolean sendUnconnected() {
        return send(SerialProtocol.CMD_UNCONNECTED);
    }

    private void processIncomingData(String data) {
        messageBuffer.append(data);
        int newlineIndex;
        while ((newlineIndex = messageBuffer.indexOf("\n")) != -1) {
            String message = messageBuffer.substring(0, newlineIndex).trim();
            messageBuffer.delete(0, newlineIndex + 1);
            if (!message.isEmpty() && messageHandler != null) {
                System.out.println("[Serial] RX: " + message);
                messageHandler.accept(message);
            }
        }
    }

    public static String[] getAvailablePorts() {
        SerialPort[] ports = SerialPort.getCommPorts();
        String[] names = new String[ports.length];
        for (int i = 0; i < ports.length; i++) {
            names[i] = ports[i].getSystemPortName();
        }
        return names;
    }
}
