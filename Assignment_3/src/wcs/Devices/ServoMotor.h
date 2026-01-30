#ifndef __SERVO_MOTOR__
#define __SERVO_MOTOR__

#include <Arduino.h>

class ServoMotor {

public:
  ServoMotor(int pin);

  void on();
  bool isOn();
  void setPosition(int angle);
  void off();
    
private:
  int pin; 
  int angle;
  bool _on;
};

#endif
