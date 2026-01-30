#ifndef __BUTTONTASK__
#define __BUTTONTASK__
#include "Task.h"

class ButtonTask : public Task {
public:
  ButtonTask(int pin);
  void init(int period) override;
  void tick() override;
  bool isPressed();
private:
  int pin;
  bool lastState;
  bool pressed;
};

#endif
