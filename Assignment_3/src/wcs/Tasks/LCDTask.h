#ifndef __LCDTASK__
#define __LCDTASK__
#include "Task.h"
#include <LiquidCrystal_I2C.h>

class LCDTask : public Task {
public:
  LCDTask(uint8_t addr, uint8_t cols, uint8_t rows);
  void init(int period) override;
  void tick() override;
  void setMode(const char* mode);
  void setValvePercent(int percent);
private:
  LiquidCrystal_I2C lcd;
  const char* mode;
  int valvePercent;
};

#endif
