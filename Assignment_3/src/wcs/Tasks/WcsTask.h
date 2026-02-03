#ifndef _WcsTask_
#define _WcsTask_

#include "Task.h"
#include "../config.h"
#include "LCDTask.h"
#include "ServoTask.h"
#include "ButtonTask.h"
#include <Arduino.h>

class WcsTask : public Task {
public:
    enum State { UNCONNECTED, AUTOMATIC, MANUAL };


private:
    State state;
    int lastState;
    LCDTask* lcd;
    ServoTask* servo;
    ButtonTask* button;
    int potPin;
    int valveLevel; // 0-100 percent
    int lastSentValveLevel; // for MANUAL mode reporting
    int manualOverrideValue; // last value set by website
    bool manualOverrideActive; // true if website set value and pot hasn't changed
    int lastPotPercent; // last potentiometer reading to detect movement
    String modeStr;

    String serialBuffer;

    bool justEnteredState(State newState);
    void updateLcd();
    void setValveLevel(int percent);
    int readPotPercent();
    void handleSerialInput();
    void sendStatusToCus();

public:
    WcsTask(LCDTask* lcd, ServoTask* servo, ButtonTask* button, int potPin);
    void init(int period) override;
    void tick() override;
    void setUnconnected();
    State getState() const { return state; }
    int getValveLevel() const { return valveLevel; }
};

#endif