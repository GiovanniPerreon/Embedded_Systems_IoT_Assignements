#ifndef _DroneHangarTask_
#define _DroneHangarTask_

#include "Task.h"
#include "../config.h"
#include "../Tasks/LCDTask.h"
#include "../Tasks/BlinkTask.h"
#include "../Tasks/ButtonTask.h"
#include "../Tasks/PIRTask.h"
#include "../Tasks/ServoTask.h"
#include "../Tasks/UltrasonicTask.h"
#include "../Tasks/TempTask.h"



class DroneHangarTask: public Task {

    enum { DRONE_INSIDE, TAKE_OFF, TAKE_OFF_CHECK, DRONE_OUT, DETECT, LANDING, LANDING_CHECK} state;
    int lastState;

    LCDTask* lcd;
    BlinkTask* led1;
    BlinkTask* led2;
    BlinkTask* led3;
    ButtonTask* button;
    PIRTask* pirSensor;
    ServoTask* servoMotor;
    UltrasonicTask* ultrasonicSensor;
    TempTask* tempTask;

    String serialBuffer;
    unsigned long takeoffCheckStartTime;
    unsigned long landingCheckStartTime;

    String readSerialCommand();
    void sendState(const char* state);
    void handleStatusRequest();
    bool justEnteredState(int newState); // Helper to detect state entry

public:
  DroneHangarTask(LCDTask* lcd, BlinkTask* led1, BlinkTask* led2, BlinkTask* led3, ButtonTask* button, PIRTask* pirSensor, ServoTask* servoMotor,
    UltrasonicTask* ultrasonicSensor, TempTask* tempTask);
  void init(int period);
  void tick();
};


#endif