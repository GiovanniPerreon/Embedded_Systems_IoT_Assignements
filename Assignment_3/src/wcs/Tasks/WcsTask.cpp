#include "WcsTask.h"

WcsTask::WcsTask(LCDTask* lcd, ServoTask* servo, ButtonTask* button, int potPin)
    : lcd(lcd), servo(servo), button(button), potPin(potPin), state(UNCONNECTED), lastState(-1), valveLevel(0) {}

void WcsTask::init(int period) {
    Task::init(period);
    state = UNCONNECTED;
    lastState = -1;
    valveLevel = 0;
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
    lcd->printLCD("Valve: " + String(valveLevel) + "%", 0, 0);
    lcd->printLCD(modeStr, 1, 0);
}

void WcsTask::setValveLevel(int percent) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    valveLevel = percent;
    int angle = map(percent, 0, 100, 0, 90);
    servo->setPosition(angle);
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
    // Button toggles MANUAL/AUTOMATIC
    if (button->isPressed()) {
        if (state == MANUAL) {
            state = AUTOMATIC;
            modeStr = "AUTOMATIC";
        } else {
            state = MANUAL;
            modeStr = "MANUAL";
        }
        updateLcd();
    }

    switch (state) {
        case UNCONNECTED:
            if (justEnteredState(UNCONNECTED)) {
                setValveLevel(0);
                modeStr = "UNCONNECTED";
                updateLcd();
            }
            break;
        case AUTOMATIC:
            if (justEnteredState(AUTOMATIC)) {
                modeStr = "AUTOMATIC";
                updateLcd();
            }
            // valveLevel is set automatically
            setValveLevel(90 - valveLevel);
            break;
        case MANUAL:
            if (justEnteredState(MANUAL)) {
                modeStr = "MANUAL";
                updateLcd();
            }
            // Read potentiometer and set valve
            setValveLevel(readPotPercent());
            updateLcd();
            break;
    }
}