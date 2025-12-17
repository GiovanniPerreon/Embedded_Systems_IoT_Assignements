#include "TempTask.h"
#include "../Devices/TempSensorLM35.h"
#include "Arduino.h"

TempTask::TempTask(int pin, ButtonTask button, ServoTask servo, BlinkTask led, LCDTask lcd) : button(button), servo(servo), led(led), lcd(lcd) {
  this->sensor = new TempSensorLM35(pin);    
}
  
void TempTask::init(int period){
  Task::init(period);
}

bool TempTask::isInAlarm() {
  return state == ALARM;
}

bool TempTask::isInPreAlarm() {
  return state == PREALARM || state == ALARM;
}
  
void TempTask::tick(){
  float temperature = sensor->getTemperature();
  
  switch(state){
    case NORMAL:
      if(temperature >= TEMP1){
        state = HIGHTEMP;
      }
      break;

    case HIGHTEMP:
      long checkStart = getCurrentTimeInState();
      while (getCurrentTimeInState() - checkStart < T3) {
        temperature = sensor->getTemperature();
        if (temperature < TEMP1) {
          state = NORMAL;
        }
      }
      state = PREALARM;
      break;

    case PREALARM:
      //disable new take offs and landings
      if(temperature >= TEMP2){
        state = VERYHIGHTEMP;
      } else if(temperature < TEMP1){
        state = NORMAL;
      }
      break;

    case VERYHIGHTEMP:
      long checkStart = getCurrentTimeInState();
      while (getCurrentTimeInState() - checkStart < T4) {
        temperature = sensor->getTemperature();
        if (temperature < TEMP2) {
          state = PREALARM;
        }
      }
      state = ALARM;
      break;

    case ALARM:
      //disable all operations
      servo.close();
      led.on();
      lcd.printLCD("ALARM", 0, 0);
      Serial.println(ALARM_MESSAGE);
      if (button.isButtonPressed()) {
        lcd.clear();
        led.off();
        state = NORMAL;
      }
      break;
  }
