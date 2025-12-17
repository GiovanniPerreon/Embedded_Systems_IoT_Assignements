#include "UltrasonicTask.h"
#include "Arduino.h"

UltrasonicTask::UltrasonicTask(int trigPin, int echoPin){
  this->sensor = new UltrasonicSensor(trigPin, echoPin);    
}
  
void UltrasonicTask::init(int period){
  Task::init(period);
}
  
void UltrasonicTask::tick(){
  float distance = sensor->getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

int UltrasonicTask::getDistance() {
    return sensor->getDistance();
}
