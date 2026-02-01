package cus.policy;

import cus.config.Config;
import cus.model.SystemMode;
import cus.model.SystemState;
import cus.serial.SerialHandler;

public class WaterLevelPolicy implements Runnable {

    private final SystemState state;
    private final SerialHandler serial;
    private volatile boolean running = true;
    private int lastSentValvePercent = -1;

    public WaterLevelPolicy(SystemState state, SerialHandler serial) {
        this.state = state;
        this.serial = serial;
    }

    @Override
    public void run() {
        System.out.println("[Policy] Started evaluation loop");
        while (running) {
            try {
                evaluate();
                Thread.sleep(Config.POLICY_EVAL_INTERVAL);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
    }

    private void evaluate() {
        SystemMode currentMode = state.getMode();

        // Check UNCONNECTED condition
        if (currentMode != SystemMode.UNCONNECTED) {
            if (state.timeSinceLastMqtt() > Config.T2) {
                System.out.println("[Policy] No MQTT data for " + Config.T2 + "ms -> UNCONNECTED");
                state.setMode(SystemMode.UNCONNECTED);
                serial.sendUnconnected();
                return;
            }
        } else {
            // Currently UNCONNECTED -- check if data resumed
            if (state.timeSinceLastMqtt() <= Config.T2) {
                System.out.println("[Policy] MQTT data resumed -> AUTOMATIC");
                state.setMode(SystemMode.AUTOMATIC);
                serial.sendModeAuto();
            } else {
                return;
            }
        }

        currentMode = state.getMode();

        if (currentMode == SystemMode.AUTOMATIC) {
            evaluateAutomatic();
        }
    }

    private void evaluateAutomatic() {
        double level = state.getLatestWaterLevel();
        int targetValve;

        if (level > Config.L2) {
            targetValve = 100;
            state.resetL1Timer();
        } else if (level > Config.L1) {
            state.startL1Timer();
            if (state.getL1ExceededDuration() >= Config.T1) {
                targetValve = 50;
            } else {
                return;
            }
        } else {
            targetValve = 0;
            state.resetL1Timer();
        }

        if (targetValve != lastSentValvePercent) {
            state.setValveOpenPercent(targetValve);
            serial.sendValveOpen(targetValve);
            lastSentValvePercent = targetValve;
            System.out.println("[Policy] Valve -> " + targetValve + "% (level=" + level + " cm)");
        }
    }

    public void stop() {
        running = false;
    }
}
