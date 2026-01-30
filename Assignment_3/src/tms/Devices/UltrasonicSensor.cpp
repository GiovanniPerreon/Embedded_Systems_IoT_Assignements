#include "UltrasonicSensor.h"
#include "Arduino.h"

UltrasonicSensor::UltrasonicSensor(int trigPin, int echoPin){
  this->trigPin = trigPin;
  this->echoPin = echoPin;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

float UltrasonicSensor::getDistance(){
  // Send 10us pulse to trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read echo pulse duration
  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
  
  // Calculate distance in cm (speed of sound = 343 m/s = 0.0343 cm/us)
  // Distance = (duration / 2) * 0.0343
  float distance = duration * 0.01715;
  
  return distance;
}
