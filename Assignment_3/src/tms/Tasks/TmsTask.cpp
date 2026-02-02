
#include "TmsTask.h"
#include <Arduino.h>

TmsTask::TmsTask(BlinkTask* greenLedTask, BlinkTask* redLedTask, UltrasonicTask* ultrasonicTask) {
  this->greenLedTask = greenLedTask;
  this->redLedTask = redLedTask;
  this->ultrasonicTask = ultrasonicTask;
  lastDistance = 0;
}

void TmsTask::init(int period) {
  Task::init(period);
  greenLedTask->off();
  redLedTask->off();
}

void TmsTask::tick() {
  ultrasonicTask->tick();
  Serial.print("[TMS] Distance: ");
  Serial.print(ultrasonicTask->getDistance());
  Serial.println(" cm");

  // if distance < threshold, turn on red LED, else green
  if (ultrasonicTask->getDistance() < 10) {
    redLedTask->on();
    greenLedTask->off();
  } else {
    greenLedTask->on();
    redLedTask->off();
  }
}
