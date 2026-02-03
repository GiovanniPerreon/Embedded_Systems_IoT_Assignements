#include "WcsTask.h"

WcsTask::WcsTask(LCDTask* lcd, ServoTask* servo, ButtonTask* button, int potPin)
    : state(UNCONNECTED), lastState(-1), lcd(lcd), servo(servo), button(button), potPin(potPin), valveLevel(0), lastSentValveLevel(-1), manualOverrideValue(-1), manualOverrideActive(false), lastPotPercent(-1) {}

void WcsTask::init(int period) {
    Task::init(period);
    state = UNCONNECTED;
    lastState = -1;
    valveLevel = 0;
    lastSentValveLevel = -1;
    manualOverrideValue = -1;
    manualOverrideActive = false;
    lastPotPercent = -1;
    modeStr = "UNCONNECTED";
}

bool WcsTask::justEnteredState(State newState) {
    if (lastState != newState) {
        lastState = newState;
        return true;
    }
    return false;
}

void WcsTask::updateLcd() {
    lcd->clear();
    String valveStr = "Valve: " + String(valveLevel) + "%";
    lcd->printLCD(valveStr, 0, 0);
    lcd->printLCD(modeStr, 0, 1);
}


void WcsTask::setValveLevel(int percent) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    valveLevel = percent;
    int angle = map(percent, 0, 100, 0, 90);
    servo->setAngle(angle);
}


int WcsTask::readPotPercent() {
    int val = analogRead(potPin);
    return map(val, 0, 1023, 0, 100);
}

void WcsTask::setUnconnected() {
    state = UNCONNECTED;
    modeStr = "UNCONNECTED";
    setValveLevel(0);
    updateLcd();
}

void WcsTask::tick() {
    // Handle incoming serial commands from CUS
    handleSerialInput();

    // Button toggles MANUAL/AUTOMATIC and notifies CUS
    if (button->isButtonPressed()) {
        if (state == MANUAL) {
            state = AUTOMATIC;
            modeStr = "AUTOMATIC";
            Serial.println(CMD_MODE_AUTO);
        } else {
            state = MANUAL;
            modeStr = "MANUAL";
            Serial.println(CMD_MODE_MANUAL);
        }
        updateLcd();
    }

    switch (state) {
        case UNCONNECTED:
            if (justEnteredState(UNCONNECTED)) {
                setValveLevel(0);
                modeStr = "UNCONNECTED";
                updateLcd();
                Serial.println(RESP_UNCONNECTED);
            }
            break;
        case AUTOMATIC:
            if (justEnteredState(AUTOMATIC)) {
                modeStr = "AUTOMATIC";
                manualOverrideActive = false;
                manualOverrideValue = -1;
                updateLcd();
                Serial.println(RESP_MODE_AUTO);
            }
            updateLcd();
            break;
        case MANUAL: {
            if (justEnteredState(MANUAL)) {
                modeStr = "MANUAL";
                updateLcd();
                Serial.println(RESP_MODE_MANUAL);
                manualOverrideActive = false;
                manualOverrideValue = -1;
            }
            int potPercent = readPotPercent();
            if (manualOverrideActive) {
                // Only resume pot control if user moves the potentiometer
                setValveLevel(manualOverrideValue);
                updateLcd();
                if (abs(potPercent - lastPotPercent) > 3) {
                    manualOverrideActive = false;
                }
            } else {
                setValveLevel(potPercent);
                updateLcd();
                if (potPercent != lastSentValveLevel) {
                    Serial.print(CMD_VALVE_OPEN);
                    Serial.println(potPercent);
                    lastSentValveLevel = potPercent;
                }
            }
            lastPotPercent = potPercent;
            break;
        }
    }
}

void WcsTask::handleSerialInput() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (serialBuffer.length() > 0) {
                String cmd = serialBuffer;
                serialBuffer = "";
                cmd.trim();
                if (cmd == CMD_MODE_MANUAL) {
                    state = MANUAL;
                    modeStr = "MANUAL";
                    Serial.println(RESP_MODE_MANUAL);
                    sendStatusToCus();
                } else if (cmd == CMD_MODE_AUTO) {
                    state = AUTOMATIC;
                    modeStr = "AUTOMATIC";
                    Serial.println(RESP_MODE_AUTO);
                    sendStatusToCus();
                } else if (cmd.startsWith(CMD_VALVE_OPEN)) {
                    int percent = cmd.substring(String(CMD_VALVE_OPEN).length()).toInt();
                    setValveLevel(percent);
                    updateLcd();
                    manualOverrideValue = percent;
                    manualOverrideActive = true;
                    Serial.print(RESP_VALVE_OPEN);
                    Serial.println(percent);
                } else if (cmd == CMD_STATUS) {
                    sendStatusToCus();
                } else if (cmd == "UNCONNECTED") {
                    setUnconnected();
                }
            }
        } else {
            serialBuffer += c;
        }
    }
}

void WcsTask::sendStatusToCus() {
    // Respond with current mode and valve level
    switch (state) {
        case MANUAL:
            Serial.println(RESP_MODE_MANUAL);
            break;
        case AUTOMATIC:
            Serial.println(RESP_MODE_AUTO);
            break;
        case UNCONNECTED:
        default:
            Serial.println(RESP_UNCONNECTED);
            break;
    }
    Serial.print(RESP_VALVE_OPEN);
    Serial.println(valveLevel);
}