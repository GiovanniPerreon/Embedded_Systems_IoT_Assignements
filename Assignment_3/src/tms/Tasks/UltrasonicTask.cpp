#include "UltrasonicTask.h"
#include "Arduino.h"


float distance = 0;
float lastValidDistance = 0;

UltrasonicTask::UltrasonicTask(int trigPin, int echoPin){
  this->sensor = new UltrasonicSensor(trigPin, echoPin); 
}
  
void UltrasonicTask::init(int period){
  Task::init(period);
}
  

void UltrasonicTask::tick(){
  float newDistance = sensor->getDistance();
  if (newDistance > 0.0) {
    distance = newDistance;
    lastValidDistance = newDistance;
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.print("Distance: ");
    Serial.print(lastValidDistance);
    Serial.println(" cm (last valid, filtered)");
    distance = lastValidDistance;
  }
}

int UltrasonicTask::getDistance() {
    return distance;
}
