#ifndef __TEMPTASK__
#define __TEMPTASK__

#include "Task.h"
#include "../Devices/TempSensor.h"

class TempTask: public Task {

  TempSensor* sensor;

public:

  TempTask(int pin);  
  void init(int period);  
  void tick();
};

#endif
