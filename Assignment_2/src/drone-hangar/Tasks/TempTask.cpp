#include "TempTask.h"
#include "../Devices/TempSensorLM35.h"
#include "Arduino.h"

TempTask::TempTask(int pin){
  this->sensor = new TempSensorLM35(pin);    
}
  
void TempTask::init(int period){
  Task::init(period);
}
  
void TempTask::tick(){
  float temperature = sensor->getTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
}
