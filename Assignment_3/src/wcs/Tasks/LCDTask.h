#ifndef __LCDTASK__
#define __LCDTASK__

#include "Task.h"
#include "../Devices/LCD.h"

class LCDTask: public Task {

  LCD* lcd;

public:

  LCDTask(int address, int cols, int rows);  
  void init(int period);  
  void tick();
  void printLCD(String text, int col, int row);
  void clear();
};

#endif
