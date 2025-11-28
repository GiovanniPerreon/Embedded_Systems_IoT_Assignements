#ifndef __LCDTASK__
#define __LCDTASK__

#include "Task.h"
#include "../Devices/LCD.h"
#include "ButtonTask.h"

class LCDTask: public Task {

  LCD* lcd;
  ButtonTask* buttonTask;

public:

  LCDTask(int address, int cols, int rows, ButtonTask* buttonTask);  
  void init(int period);  
  void tick();
};

#endif
