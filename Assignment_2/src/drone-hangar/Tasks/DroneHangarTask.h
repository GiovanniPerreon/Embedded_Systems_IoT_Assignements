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

    LCDTask lcd;
    BlinkTask led;
    ButtonTask button;
    PIRTask pirSensor;
    ServoTask servoMotor;
    UltrasonicTask ultrasonicSensor;
    TempTask tempTask;

public: 
  DroneHangarTask(LCDTask lcd, BlinkTask led, ButtonTask button, PIRTask pirSensor, ServoTask servoMotor, 
    UltrasonicTask ultrasonicSensor, TempTask tempTask);
  void init(int period);
  void tick();
};


#endif