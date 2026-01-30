#include "ServoTask.h"
#include <Arduino.h>

ServoTask::ServoTask(int servoPin, int potPin) : servoPin(servoPin), potPin(potPin), positionPercent(0), manualMode(false) {}

void ServoTask::init(int period) {
  servo.attach(servoPin);
}

void ServoTask::tick() {
  if (manualMode) {
    int potValue = analogRead(potPin);
    positionPercent = map(potValue, 0, 1023, 0, 100);
    int angle = map(positionPercent, 0, 100, 0, 90);
    servo.write(angle);
  }
}

void ServoTask::setPositionPercent(int percent) {
  manualMode = false;
  positionPercent = constrain(percent, 0, 100);
  int angle = map(positionPercent, 0, 100, 0, 90);
  servo.write(angle);
}

int ServoTask::getPositionPercent() {
  return positionPercent;
}
