package cus.serial;

import cus.config.Config;

public final class SerialProtocol {

    private SerialProtocol() {}

    // Connection settings
    public static final int BAUD_RATE = Config.SERIAL_BAUD;
    public static final int DATA_BITS = Config.SERIAL_DATA_BITS;
    public static final int STOP_BITS = Config.SERIAL_STOP_BITS;
    public static final int PARITY = Config.SERIAL_PARITY;
    public static final int READ_TIMEOUT_MS = Config.SERIAL_READ_TIMEOUT_MS;
    public static final int WRITE_TIMEOUT_MS = Config.SERIAL_WRITE_TIMEOUT_MS;

    // Commands CUS -> WCS
    public static final String CMD_MODE_MANUAL = "MODE_MANUAL";
    public static final String CMD_MODE_AUTO = "MODE_AUTO";
    public static final String CMD_VALVE_OPEN = "VALVE_OPEN:";
    public static final String CMD_STATUS = "STATUS?";
    public static final String CMD_UNCONNECTED = "UNCONNECTED";

    // Responses WCS -> CUS
    public static final String RESP_MANUAL = "MANUAL";
    public static final String RESP_AUTOMATIC = "AUTOMATIC";
    public static final String RESP_UNCONNECTED = "UNCONNECTED";
    public static final String RESP_VALVE_OPEN = "VALVE_OPEN:";
}
