#include "UltrasonicTask.h"
#include "Arduino.h"

float distance = 0;   

UltrasonicTask::UltrasonicTask(int trigPin, int echoPin){
  this->sensor = new UltrasonicSensor(trigPin, echoPin); 
}
  
void UltrasonicTask::init(int period){
  Task::init(period);
}
  
void UltrasonicTask::tick(){
  distance = sensor->getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

int UltrasonicTask::getDistance() {
    return distance;
}
