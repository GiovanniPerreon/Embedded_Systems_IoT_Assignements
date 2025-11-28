#include "TempSensorTMP36.h"
#include <Arduino.h>

TempSensorTMP36::TempSensorTMP36(int pin){
  this->pin = pin;
}

float TempSensorTMP36::getTemperature(){
  int reading = analogRead(pin);
  float voltage = reading * (5.0 / 1024.0);
  float temperatureC = (voltage - 0.5) * 100.0;
  return temperatureC;
}
