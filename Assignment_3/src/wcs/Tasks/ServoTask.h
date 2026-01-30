#ifndef __SERVOTASK__
#define __SERVOTASK__
#include "Task.h"
#include <Servo.h>

class ServoTask : public Task {
public:
  ServoTask(int servoPin, int potPin);
  void init(int period) override;
  void tick() override;
  void setPositionPercent(int percent);
  int getPositionPercent();
private:
  int servoPin;
  int potPin;
  Servo servo;
  int positionPercent;
  bool manualMode;
};

#endif
