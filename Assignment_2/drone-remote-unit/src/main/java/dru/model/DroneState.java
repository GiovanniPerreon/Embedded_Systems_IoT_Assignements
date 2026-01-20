package dru.model;

/**
 * Drone Remote Unit state machine states.
 *
 * State transitions:
 * - DRONE_REST -> DRONE_TAKEOFF (on TAKEOFF command sent and confirmed)
 * - DRONE_TAKEOFF -> DRONE_OPERATING (when drone exits hangar)
 * - DRONE_OPERATING -> DRONE_LANDING (on LAND command sent and confirmed)
 * - DRONE_LANDING -> DRONE_REST (when drone lands inside hangar)
 *
 * Any state can transition to ALARM when alarm is received.
 * ALARM -> DRONE_REST (after reset on Arduino side)
 */
public enum DroneState {

    DRONE_REST("At Rest", "Drone is inside hangar"),
    DRONE_TAKEOFF("Taking Off", "Drone is exiting hangar"),
    DRONE_OPERATING("Operating", "Drone is outside flying"),
    DRONE_LANDING("Landing", "Drone is entering hangar"),
    ALARM("ALARM", "System alarm - temperature critical");

    private final String displayName;
    private final String description;

    DroneState(String displayName, String description) {
        this.displayName = displayName;
        this.description = description;
    }

    public String getDisplayName() {
        return displayName;
    }

    public String getDescription() {
        return description;
    }

    /**
     * Check if TAKEOFF command is allowed in current state.
     */
    public boolean canTakeoff() {
        return this == DRONE_REST;
    }

    /**
     * Check if LAND command is allowed in current state.
     */
    public boolean canLand() {
        return this == DRONE_OPERATING;
    }

    /**
     * Check if this is an alarm state.
     */
    public boolean isAlarm() {
        return this == ALARM;
    }
}
