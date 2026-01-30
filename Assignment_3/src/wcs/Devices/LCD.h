#ifndef __LCD__
#define __LCD__

#include <LiquidCrystal_I2C.h>

class LCD {
  
  LiquidCrystal_I2C* lcd;
  int cols;
  int rows;

public:
  LCD(int address, int cols, int rows);
  void init();
  void clear();
  void setCursor(int col, int row);
  void print(String text);
  void print(int value);
  void print(float value);
};

#endif
