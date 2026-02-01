package cus.model;

import cus.config.Config;
import java.util.LinkedList;
import java.util.List;

public class SystemState {

    private SystemMode mode;
    private int valveOpenPercent;
    private final LinkedList<WaterLevelReading> waterLevelHistory;
    private long lastMqttTimestamp;
    private long l1ExceededSince;
    private boolean l1TimerActive;

    public SystemState() {
        this.mode = SystemMode.AUTOMATIC;
        this.valveOpenPercent = 0;
        this.waterLevelHistory = new LinkedList<>();
        this.lastMqttTimestamp = System.currentTimeMillis();
        this.l1ExceededSince = 0;
        this.l1TimerActive = false;
    }

    // --- Mode ---

    public synchronized SystemMode getMode() {
        return mode;
    }

    public synchronized void setMode(SystemMode newMode) {
        this.mode = newMode;
    }

    // --- Valve ---

    public synchronized int getValveOpenPercent() {
        return valveOpenPercent;
    }

    public synchronized void setValveOpenPercent(int percent) {
        this.valveOpenPercent = Math.max(0, Math.min(100, percent));
    }

    // --- Water Level History ---

    public synchronized void addWaterLevel(double level) {
        waterLevelHistory.addLast(new WaterLevelReading(level, System.currentTimeMillis()));
        while (waterLevelHistory.size() > Config.N) {
            waterLevelHistory.removeFirst();
        }
        lastMqttTimestamp = System.currentTimeMillis();
    }

    public synchronized List<WaterLevelReading> getWaterLevelHistory() {
        return new LinkedList<>(waterLevelHistory);
    }

    public synchronized double getLatestWaterLevel() {
        if (waterLevelHistory.isEmpty()) return 0.0;
        return waterLevelHistory.getLast().level();
    }

    // --- MQTT Timeout ---

    public synchronized long getLastMqttTimestamp() {
        return lastMqttTimestamp;
    }

    public synchronized long timeSinceLastMqtt() {
        return System.currentTimeMillis() - lastMqttTimestamp;
    }

    // --- L1 Timer ---

    public synchronized void startL1Timer() {
        if (!l1TimerActive) {
            l1ExceededSince = System.currentTimeMillis();
            l1TimerActive = true;
        }
    }

    public synchronized void resetL1Timer() {
        l1ExceededSince = 0;
        l1TimerActive = false;
    }

    public synchronized boolean isL1TimerActive() {
        return l1TimerActive;
    }

    public synchronized long getL1ExceededDuration() {
        if (!l1TimerActive) return 0;
        return System.currentTimeMillis() - l1ExceededSince;
    }

    // --- Snapshot for HTTP ---

    public synchronized StateSnapshot snapshot() {
        return new StateSnapshot(
            mode,
            valveOpenPercent,
            new LinkedList<>(waterLevelHistory),
            getLatestWaterLevel()
        );
    }

    // --- Records ---

    public record WaterLevelReading(double level, long timestamp) {}

    public record StateSnapshot(
        SystemMode mode,
        int valveOpenPercent,
        List<WaterLevelReading> waterLevelHistory,
        double latestWaterLevel
    ) {}
}
