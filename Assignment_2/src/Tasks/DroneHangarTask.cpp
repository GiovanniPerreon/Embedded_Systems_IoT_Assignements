#include "DroneHangarTask.h"
#include "Arduino.h"
#include "../Kernel/kernel.h"

DroneHangarTask::DroneHangarTask(LCDTask* lcd, BlinkTask* led1, BlinkTask* led2, BlinkTask* led3, ButtonTask* button, PIRTask* pirSensor,
  ServoTask* servoMotor, UltrasonicTask* ultrasonicSensor, TempTask* tempTask)
: lcd(lcd), led1(led1), led2(led2), led3(led3), button(button), pirSensor(pirSensor), servoMotor(servoMotor), ultrasonicSensor(ultrasonicSensor), tempTask(tempTask) {
  serialBuffer = "";
}

void DroneHangarTask::init(int period){
  Task::init(period);
  state = DRONE_INSIDE;
  lastState = -1;
  takeoffCheckStartTime = 0;
  landingCheckStartTime = 0;
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

bool DroneHangarTask::justEnteredState(int newState) {
  if (lastState != newState) {
    lastState = newState;
    return true;
  }
  return false;
}

void DroneHangarTask::tick(){
  String cmd = readSerialCommand();

  // Handle status request in any state
  if (cmd == CMD_STATUS) {
    if (tempTask->isInAlarm()) {
      sendState(RESP_ALARM);
    } else {
      handleStatusRequest();
    }
    return;
  }
  if (tempTask->isInPreAlarm() && !tempTask->isInAlarm()) {
    sendState(RESP_PREALARM);
  } else if (!tempTask->isInPreAlarm() && !tempTask->isInAlarm() && !tempTask->isHighTemp()) {
    sendState(RESP_NORMAL);
  }
  if (!tempTask->isInAlarm()) {
    switch(state){
      case DRONE_INSIDE:
        if (justEnteredState(DRONE_INSIDE)) {
          lcd->clear();
          lcd->printLCD("DRONE INSIDE", 0, 0);
          led1->on();
          led2->off();
          led3->off();
        }
        if (cmd == CMD_TAKEOFF && !tempTask->isInPreAlarm()) {
          sendState(RESP_TAKEOFF);
          state = TAKE_OFF;
        }
        break;

      case TAKE_OFF:
        servoMotor->open();
        if (justEnteredState(TAKE_OFF)) {
          led2->blink();
          lcd->clear();
          lcd->printLCD("TAKE OFF", 0, 0);
        }
        if (ultrasonicSensor->getDistance() > D1) {
          state = TAKE_OFF_CHECK;
        }
        break;

      case TAKE_OFF_CHECK:
        if (justEnteredState(TAKE_OFF_CHECK)) {
          takeoffCheckStartTime = millis();
        }
        
        if (ultrasonicSensor->getDistance() < D1) {
          // Distance dropped below threshold, drone didn't leave
          led2->off();
          servoMotor->close();
          sendState(RESP_INSIDE);
          state = DRONE_INSIDE;
        } else if (millis() - takeoffCheckStartTime >= T1) {
          // Distance stayed above D1 for T1 seconds
          led2->off();
          servoMotor->close();
          lcd->clear();
          sendState(RESP_OUTSIDE);
          state = DRONE_OUT;
        }
        break;

      case DRONE_OUT:
        if (justEnteredState(DRONE_OUT)) {
          lcd->clear();
          lcd->printLCD("DRONE OUT", 0, 0);
          led2->off(); // LED should not blink when drone is outside
        }
        if (cmd == CMD_LAND && !tempTask->isInPreAlarm()) {
          state = DETECT;
        }
        break;

      case DETECT:
        if (justEnteredState(DETECT)) {
        }
        if(pirSensor->isMotionDetected()) {
          servoMotor->open();
          sendState(RESP_LANDING);
          state = LANDING;
        }
        break;

      case LANDING: {
        if (justEnteredState(LANDING)) {
          led2->blink();
          // lcd->clear();
          // lcd->printLCD("Landing", 0, 0);
        }
        float distance = ultrasonicSensor->getDistance();
        Serial.print(RESP_DIST_PREFIX);
        Serial.println(distance);
        if (distance < D2) {
          landingCheckStartTime = millis();
          state = LANDING_CHECK;
        }
        break;
      }

      case LANDING_CHECK:
        if (justEnteredState(LANDING_CHECK)) {
          // lcd->clear();
          // lcd->printLCD("Landing check", 0, 0);
        }
        
        float distance = ultrasonicSensor->getDistance();
        Serial.print(RESP_DIST_PREFIX);
        Serial.println(distance);
        
        if (distance > D2) {
          // Distance increased, drone bounced up
          state = LANDING;
        } else if (millis() - landingCheckStartTime >= T2) {
          // Distance stayed below D2 for T2 seconds, drone landed
          led2->off();
          lcd->clear();
          servoMotor->close();
          sendState(RESP_INSIDE);
          state = DRONE_INSIDE;
        }
        break;
    }
  }
}
