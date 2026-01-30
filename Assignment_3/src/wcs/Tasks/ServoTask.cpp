#include "ServoTask.h"
#include "Arduino.h"

ServoTask::ServoTask(int pin){
  this->pin = pin;    
  targetAngle = 0;
}

void ServoTask::init(int period){
  Task::init(period);
  servo = new ServoMotor(pin);
  servo->on();
  targetAngle = 0;
  servo->setPosition(targetAngle);
}

void ServoTask::tick(){
  servo->setPosition(targetAngle);
}

void ServoTask::setAngle(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  targetAngle = angle;
}

int ServoTask::getAngle() {
  return targetAngle;
}
