#ifndef __POTENTIOMETER__
#define __POTENTIOMETER__
#include <Arduino.h>

class Potentiometer {
public:
  Potentiometer(int pin);
  void init();
  int readValue();
private:
  int pin;
};

#endif
