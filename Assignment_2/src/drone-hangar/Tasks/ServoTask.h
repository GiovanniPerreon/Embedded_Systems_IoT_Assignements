#ifndef __SERVOTASK__
#define __SERVOTASK__

#include "Task.h"
#include "../Devices/ServoMotor.h"

class ServoTask: public Task {

  int pin;
  enum { AT_0, AT_180 } state;
  ServoMotor* servo;

public:

  ServoTask(int pin);  
  void init(int period);  
  void tick();
  void open();
  void close();
};

#endif
