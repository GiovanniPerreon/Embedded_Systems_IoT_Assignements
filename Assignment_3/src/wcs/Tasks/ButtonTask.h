#ifndef __BUTTONTASK__
#define __BUTTONTASK__

#include "Task.h"
#include "../Devices/Button.h"

class ButtonTask: public Task {

  int pin;
  enum { PRESSED, NOT_PRESSED} state;
  Button* button;
  bool pressedEventFlag;  // Flag for single press event

public:

  ButtonTask(int pin);  
  void init(int period);  
  void tick();
  bool isButtonPressed();  // Returns true only once per press
};

#endif
