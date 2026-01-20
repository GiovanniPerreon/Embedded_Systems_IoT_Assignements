#include "DroneHangarTask.h"
#include "Arduino.h"
#include "../Kernel/kernel.h"

DroneHangarTask::DroneHangarTask(LCDTask lcd, BlinkTask led, ButtonTask button, PIRTask pirSensor,
  ServoTask servoMotor, UltrasonicTask ultrasonicSensor, TempTask tempTask)
: lcd(lcd), led(led), button(button), pirSensor(pirSensor), servoMotor(servoMotor), ultrasonicSensor(ultrasonicSensor), tempTask(tempTask) {
  serialBuffer = "";
}

void DroneHangarTask::init(int period){
  Task::init(period);
  state = DRONE_INSIDE;
}

String DroneHangarTask::readSerialCommand() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      String cmd = serialBuffer;
      cmd.trim();
      serialBuffer = "";
      return cmd;
    } else {
      serialBuffer += c;
    }
  }
  return "";
}

void DroneHangarTask::sendState(const char* stateStr) {
  Serial.println(stateStr);
}

void DroneHangarTask::handleStatusRequest() {
  switch(state) {
    case DRONE_INSIDE:
      sendState(RESP_INSIDE);
      break;
    case TAKE_OFF:
    case TAKE_OFF_CHECK:
      sendState(RESP_TAKEOFF);
      break;
    case DRONE_OUT:
    case DETECT:
      sendState(RESP_OUTSIDE);
      break;
    case LANDING:
    case LANDING_CHECK:
      sendState(RESP_LANDING);
      break;
  }
}

void DroneHangarTask::tick(){
  String cmd = readSerialCommand();

  // Handle status request in any state
  if (cmd == CMD_STATUS) {
    if (tempTask.isInAlarm()) {
      sendState(RESP_ALARM);
    } else {
      handleStatusRequest();
    }
    return;
  }

  if (!tempTask.isInAlarm()) {
    switch(state){
      case DRONE_INSIDE:
        lcd.clear();
        lcd.printLCD("Drone Inside", 0, 0);
        if (cmd == CMD_TAKEOFF && !tempTask.isInPreAlarm()) {
          sendState(RESP_TAKEOFF);
          state = TAKE_OFF;
        }
        break;

      case TAKE_OFF:
        servoMotor.open();
        lcd.clear();
        lcd.printLCD("Taking Off", 0, 0);
        led.on();
        if (ultrasonicSensor.getDistance() > D1) {
          state = TAKE_OFF_CHECK;
        }
        break;

      case TAKE_OFF_CHECK: {
        long checkStart = getCurrentTimeInState();
        lcd.clear();
        lcd.printLCD("Take off check", 0, 0);
        led.on();
        while (getCurrentTimeInState() - checkStart < T1) {
          if (ultrasonicSensor.getDistance() < D1) {
            led.off();
            servoMotor.close();
            sendState(RESP_INSIDE);
            state = DRONE_INSIDE;
            return;
          }
        }
        led.off();
        servoMotor.close();
        lcd.clear();
        sendState(RESP_OUTSIDE);
        state = DRONE_OUT;
        break;
      }

      case DRONE_OUT:
        lcd.clear();
        lcd.printLCD("Drone Out", 0, 0);
        if (cmd == CMD_LAND && !tempTask.isInPreAlarm()) {
          state = DETECT;
        }
        break;

      case DETECT:
        lcd.clear();
        lcd.printLCD("Detecting...", 0, 0);
        if(pirSensor.isMotionDetected()) {
          servoMotor.open();
          sendState(RESP_LANDING);
          state = LANDING;
        }
        break;

      case LANDING: {
        lcd.clear();
        lcd.printLCD("Landing", 0, 0);
        led.on();
        float distance = ultrasonicSensor.getDistance();
        Serial.print(RESP_DIST_PREFIX);
        Serial.println(distance);
        if (distance < D2) {
          state = LANDING_CHECK;
        }
        break;
      }

      case LANDING_CHECK: {
        long landingCheckStart = getCurrentTimeInState();
        while (getCurrentTimeInState() - landingCheckStart < T2) {
          float distance = ultrasonicSensor.getDistance();
          Serial.print(RESP_DIST_PREFIX);
          Serial.println(distance);
          if (distance > D2) {
            state = LANDING;
            return;
          }
        }
        led.off();
        lcd.clear();
        servoMotor.close();
        sendState(RESP_INSIDE);
        state = DRONE_INSIDE;
        break;
      }
    }
  }
}
