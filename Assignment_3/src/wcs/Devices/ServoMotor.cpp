#include "ServoMotor.h"
#include <Arduino.h>

ServoMotor::ServoMotor(int pin){
  this->pin = pin;  
  angle = 0;
  _on = false;
  pinMode(pin, OUTPUT);
} 

void ServoMotor::on(){
  _on = true;
}

bool ServoMotor::isOn(){
  return _on;
}

void ServoMotor::setPosition(int newAngle){
  angle = newAngle;
  if (angle > 180){
    angle = 180;
  } else if (angle < 0){
    angle = 0;
  }
  if (_on) {
    int pulseWidth = map(angle, 0, 180, 600, 2500);
    digitalWrite(pin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(pin, LOW);
    delay(20 - (pulseWidth / 1000));
  }
}

void ServoMotor::off(){
  _on = false;
  digitalWrite(pin, LOW);
}
