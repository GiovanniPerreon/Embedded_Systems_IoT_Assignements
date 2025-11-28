#ifndef __BUTTONTASK__
#define __BUTTONTASK__

#include "Task.h"
#include "../Devices/Button.h"

class ButtonTask: public Task {

  int pin;
  enum { PRESSED, NOT_PRESSED} state;
  Button* button;

public:

  ButtonTask(int pin);  
  void init(int period);  
  void tick();
  bool isButtonPressed();
};

#endif
