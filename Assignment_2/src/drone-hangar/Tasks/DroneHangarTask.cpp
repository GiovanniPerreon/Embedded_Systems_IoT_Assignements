#include "DroneHangarTask.h"
#include "Arduino.h"
#include "../Kernel/kernel.h"

DroneHangarTask::DroneHangarTask(LCDTask lcd, BlinkTask led, ButtonTask button, PIRTask pirSensor, 
  ServoTask servoMotor, UltrasonicTask ultrasonicSensor, TempTask tempTask)
: lcd(lcd), led(led), button(button), pirSensor(pirSensor), servoMotor(servoMotor), ultrasonicSensor(ultrasonicSensor), tempTask(tempTask) {
}

void DroneHangarTask::init(int period){
  Task::init(period);
  state = DRONE_INSIDE;
}

void DroneHangarTask::tick(){
  if (!tempTask.isInAlarm()) {
    switch(state){
      case DRONE_INSIDE:
        lcd.printLCD("Drone Inside", 0, 0);
        if (Serial.read() == OPEN_MESSAGE && !tempTask.isInPreAlarm()) {
          state = TAKE_OFF;
        }
        break;

      case TAKE_OFF:
        servoMotor.open();
        lcd.printLCD("Taking Off", 0, 0);
        led.on();
        if (ultrasonicSensor.getDistance() > D1) {
          state = TAKE_OFF_CHECK;
        }
        break;

      case TAKE_OFF_CHECK:
        long checkStart = getCurrentTimeInState();
        while (getCurrentTimeInState() - checkStart < T1) {
          if (ultrasonicSensor.getDistance() < D1) {
            led.off();
            servoMotor.close();
            state = DRONE_INSIDE;
          }
        }
        led.off();
        servoMotor.close();
        lcd.clear();
        state = DRONE_OUT;
        break;

      case DRONE_OUT:
        if (Serial.read() == OPEN_MESSAGE && !tempTask.isInPreAlarm()) {
          state = DETECT;
        }
        break;

      case DETECT:
        if(pirSensor.isMotionDetected()) {
          servoMotor.open();
          state = LANDING;
        }
        break;

      case LANDING:
        lcd.printLCD("Landing", 0, 0);
        led.on();
        Serial.println("Distace: " + String(ultrasonicSensor.getDistance()));
        if (ultrasonicSensor.getDistance() < D2) {
          state = LANDING_CHECK;
        }
        break;

      case LANDING_CHECK:
        long landingCheckStart = getCurrentTimeInState();
        while (getCurrentTimeInState() - landingCheckStart < T2) {
          Serial.println("Distace: " + String(ultrasonicSensor.getDistance()));
          if (ultrasonicSensor.getDistance() > D2) {
            state = DRONE_OUT;;
          }
        }
        led.off();
        lcd.clear();
        servoMotor.close();
        state = DRONE_INSIDE;
        break;
    }
  }
}