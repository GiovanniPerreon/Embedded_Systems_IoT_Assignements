#ifndef __PIRTASK__
#define __PIRTASK__

#include "Task.h"
#include "../Devices/PIRSensor.h"

class PIRTask: public Task {

  PIRSensor* sensor;
  bool lastMotionState;

public:

  PIRTask(int pin);  
  void init(int period);  
  void tick();
};

#endif
