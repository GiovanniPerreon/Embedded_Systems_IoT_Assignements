#include "TempSensorLM35.h"
#include <Arduino.h>

TempSensorLM35::TempSensorLM35(int pin){
  this->pin = pin;
}

float TempSensorLM35::getTemperature(){
  int reading = analogRead(pin);
  float voltage = reading * (5.0 / 1024.0);
  float temperatureC = voltage * 100.0;
  return temperatureC;
}
