package dru.serial;

/**
 * Serial protocol constants for communication with Arduino Drone Hangar.
 *
 * Protocol specification:
 * - Baud rate: 9600
 * - Line ending: \n (newline)
 *
 * Commands (PC -> Arduino):
 * - TAKEOFF : Request hangar door open for takeoff
 * - LAND    : Request hangar door open for landing
 * - STATUS? : Request current status
 *
 * Responses (Arduino -> PC):
 * - INSIDE   : Drone is inside hangar at rest
 * - OUTSIDE  : Drone is outside operating
 * - TAKEOFF  : Drone is taking off
 * - LANDING  : Drone is landing
 * - ALARM    : System is in alarm state
 * - DIST:<value> : Current distance reading (cm) during landing
 */
public final class SerialProtocol {

    private SerialProtocol() {} // Prevent instantiation

    // Connection settings
    public static final int BAUD_RATE = 9600;
    public static final int DATA_BITS = 8;
    public static final int STOP_BITS = 1;
    public static final int PARITY = 0; // None

    // Commands (PC -> Arduino)
    public static final String CMD_TAKEOFF = "TAKEOFF";
    public static final String CMD_LAND = "LAND";
    public static final String CMD_STATUS = "STATUS?";

    // Responses (Arduino -> PC)
    public static final String RESP_INSIDE = "INSIDE";
    public static final String RESP_OUTSIDE = "OUTSIDE";
    public static final String RESP_TAKEOFF = "TAKEOFF";
    public static final String RESP_LANDING = "LANDING";
    public static final String RESP_ALARM = "ALARM";
    public static final String RESP_DIST_PREFIX = "DIST:";

    // Timeouts
    public static final int READ_TIMEOUT_MS = 100;
    public static final int WRITE_TIMEOUT_MS = 100;
}
