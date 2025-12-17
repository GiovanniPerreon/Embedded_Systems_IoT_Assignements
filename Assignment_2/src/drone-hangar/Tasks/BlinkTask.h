#ifndef __BLINKTASK__
#define __BLINKTASK__

#include "Task.h"
#include "../Devices/Led.h"

class BlinkTask: public Task {

  int pin;
  Led* led;
  enum { ON, OFF} state;

public:

  BlinkTask(int pin);  
  void init(int period);  
  void tick();
  void on();
  void off();
};

#endif

