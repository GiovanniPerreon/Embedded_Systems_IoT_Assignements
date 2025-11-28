#ifndef __ULTRASONICTASK__
#define __ULTRASONICTASK__

#include "Task.h"
#include "../Devices/UltrasonicSensor.h"

class UltrasonicTask: public Task {

  UltrasonicSensor* sensor;

public:

  UltrasonicTask(int trigPin, int echoPin);  
  void init(int period);  
  void tick();
};

#endif
