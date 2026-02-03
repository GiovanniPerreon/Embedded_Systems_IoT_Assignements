package cus.config;

public final class Config {

    private Config() {}

    // Water Level Policy
    public static final double L1 = 30.0;
    public static final double L2 = 50.0;
    public static final long T1 = 5000;
    public static final long T2 = 10000;
    public static final int N = 50;

    // MQTT
    public static final String MQTT_BROKER = "tcp://test.mosquitto.org:1883";
    public static final String MQTT_TOPIC = "tank/level";
    public static final String MQTT_CLIENT_ID = "CUS";
    public static final int MQTT_QOS = 1;

    // Serial (WCS)
    public static final String SERIAL_PORT = "COM3";
    public static final int SERIAL_BAUD = 9600;
    public static final int SERIAL_DATA_BITS = 8;
    public static final int SERIAL_STOP_BITS = 1;
    public static final int SERIAL_PARITY = 0;
    public static final int SERIAL_READ_TIMEOUT_MS = 100;
    public static final int SERIAL_WRITE_TIMEOUT_MS = 100;

    // HTTP
    public static final int HTTP_PORT = 8080;

    // Policy evaluation interval (ms)
    public static final long POLICY_EVAL_INTERVAL = 500;
}
