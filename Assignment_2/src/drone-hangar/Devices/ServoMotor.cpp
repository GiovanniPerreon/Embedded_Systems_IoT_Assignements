#include "ServoMotor.h"
#include <Arduino.h>

ServoMotor::ServoMotor(int pin){
  this->pin = pin;  
  _on = false;
  pinMode(pin, OUTPUT);
} 

void ServoMotor::on(){
  _on = true;
}

bool ServoMotor::isOn(){
  return _on;
}

void ServoMotor::setPosition(int angle){
  if (angle > 180){
    angle = 180;
  } else if (angle < 0){
    angle = 0;
  }
  if (_on) {
    // Convert angle (0-180) to pulse width (calibrated: shifted 20° right)
    int pulseWidth = map(angle, 0, 180, 600, 2500);
    digitalWrite(pin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(pin, LOW);
    delay(20 - (pulseWidth / 1000)); // 20ms period
  }
}

void ServoMotor::off(){
  _on = false;
  digitalWrite(pin, LOW);
}
