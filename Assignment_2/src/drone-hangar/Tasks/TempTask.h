#ifndef __TEMPTASK__
#define __TEMPTASK__

#include "Task.h"
#include "../Devices/TempSensor.h"
#include "../config.h"
#include "../Kernel/kernel.h"
#include "../Tasks/ButtonTask.h"
#include "../Tasks/ServoTask.h"
#include "../Tasks/BlinkTask.h"
#include "../Tasks/LCDTask.h"

class TempTask: public Task {

  enum { NORMAL, HIGHTEMP, PREALARM, VERYHIGHTEMP, ALARM} state;

  TempSensor* sensor;
  ButtonTask button;
  ServoTask servo;
  BlinkTask led;
  LCDTask lcd;

public:

  TempTask(int pin, ButtonTask button, ServoTask servo, BlinkTask led, LCDTask lcd);  
  void init(int period);  
  void tick();
  bool isInAlarm();
  bool isInPreAlarm();
};

#endif
