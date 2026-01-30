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
    String modeStr;

    bool justEnteredState(State newState);
    void updateLcd();
    void setValveLevel(int percent);
    int readPotPercent();

public:
    WcsTask(LCDTask* lcd, ServoTask* servo, ButtonTask* button, int potPin);
    void init(int period) override;
    void tick() override;
    void setAutomaticLevel(int percent); // Called by CUS or external logic
    void setUnconnected();
    State getState() const { return state; }
    int getValveLevel() const { return valveLevel; }
};

#endif