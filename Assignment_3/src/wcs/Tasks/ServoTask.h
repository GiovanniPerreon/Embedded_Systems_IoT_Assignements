#ifndef __SERVOTASK__
#define __SERVOTASK__

#include "Task.h"
#include "../Devices/ServoMotor.h"

class ServoTask: public Task {
  int pin;
  ServoMotor* servo;
  int targetAngle = 0;
public:
  ServoTask(int pin);  
  void init(int period);  
  void tick();
  void setAngle(int angle); // Set servo to any angle
  int getAngle(); // Get current target angle
};

#endif
