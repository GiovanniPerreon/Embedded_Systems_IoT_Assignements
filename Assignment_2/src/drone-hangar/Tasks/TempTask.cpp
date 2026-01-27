#include "TempTask.h"
#include "../Devices/TempSensorLM35.h"
#include "Arduino.h"

TempTask::TempTask(int pin, ButtonTask* button, ServoTask* servo, BlinkTask* led, LCDTask* lcd) 
  : button(button), servo(servo), led(led), lcd(lcd) {
  this->sensor = new TempSensorLM35(pin);
  highTempStartTime = 0;
  veryHighTempStartTime = 0;
}
  
void TempTask::init(int period){
  Task::init(period);
  state = NORMAL;
}

bool TempTask::isInAlarm() {
  return state == ALARM;
}

bool TempTask::isInPreAlarm() {
  return state == PREALARM || state == ALARM;
}
  
void TempTask::tick(){
  float temperature = sensor->getTemperature();
  Serial.print("TEMP: ");
  Serial.print(temperature);
  Serial.println(" Celsius");
  switch(state){
    case NORMAL:
      if(temperature >= Temp1){
        highTempStartTime = millis();
        state = HIGHTEMP;
      }
      break;
      
    case HIGHTEMP:
      if (temperature < Temp1) {
        state = NORMAL;
      } else if (millis() - highTempStartTime >= T3) {
        state = PREALARM;
      }
      break;
    case PREALARM:
      //disable new take offs and landings
      if(temperature >= Temp2){
        veryHighTempStartTime = millis();
        state = VERYHIGHTEMP;
      } else if(temperature < Temp1){
        state = NORMAL;
      }
      break;
      
    case VERYHIGHTEMP:
      if (temperature < Temp2) {
        state = PREALARM;
      } else if (millis() - veryHighTempStartTime >= T4) {
        state = ALARM;
      }
      break;
    case ALARM:
      //disable all operations
      servo->close();
      led->on();
      lcd->clear();
      lcd->printLCD("ALARM", 0, 0);
      Serial.println(RESP_ALARM);
      if (button->isButtonPressed()) {
        lcd->clear();
        led->off();
        state = NORMAL;
      }
      break;
  }
}
