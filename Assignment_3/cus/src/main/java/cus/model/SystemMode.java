package cus.model;

public enum SystemMode {
    AUTOMATIC("AUTOMATIC"),
    MANUAL("MANUAL"),
    UNCONNECTED("UNCONNECTED");

    private final String label;

    SystemMode(String label) {
        this.label = label;
    }

    public String getLabel() {
        return label;
    }
}
