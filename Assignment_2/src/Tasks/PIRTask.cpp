#include "PIRTask.h"
#include "Arduino.h"

PIRTask::PIRTask(int pin){
  this->sensor = new PIRSensor(pin);
  this->lastMotionState = false;
}
  
void PIRTask::init(int period){
  Task::init(period);
}
  
void PIRTask::tick(){
  bool motionDetected = sensor->isMotionDetected();
  
  // Only print when state changes
  if (motionDetected != lastMotionState){
    if (motionDetected){
      Serial.println("Motion DETECTED!");
    } else {
      Serial.println("Motion stopped");
    }
    lastMotionState = motionDetected;
  }
}

boolean PIRTask::isMotionDetected() {
  return sensor->isMotionDetected();
}
