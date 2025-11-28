#ifndef __ULTRASONICSENSOR__
#define __ULTRASONICSENSOR__

class UltrasonicSensor {
  
  int trigPin;
  int echoPin;

public:
  UltrasonicSensor(int trigPin, int echoPin);
  float getDistance(); // Returns distance in cm
};

#endif
