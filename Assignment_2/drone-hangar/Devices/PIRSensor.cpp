#include "PIRSensor.h"
#include "Arduino.h"

PIRSensor::PIRSensor(int pin){
  this->pin = pin;
  pinMode(pin, INPUT);
}

bool PIRSensor::isMotionDetected(){
  return digitalRead(pin) == HIGH;
}
