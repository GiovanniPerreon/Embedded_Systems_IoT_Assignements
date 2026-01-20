package dru.model;

/**
 * Hangar operational state.
 */
public enum HangarState {

    NORMAL("Normal", "System operating normally"),
    PREALARM("Pre-Alarm", "Temperature elevated - new operations suspended"),
    ALARM("ALARM", "Critical temperature - all operations suspended");

    private final String displayName;
    private final String description;

    HangarState(String displayName, String description) {
        this.displayName = displayName;
        this.description = description;
    }

    public String getDisplayName() {
        return displayName;
    }

    public String getDescription() {
        return description;
    }

    public boolean isAlarm() {
        return this == ALARM;
    }
}
