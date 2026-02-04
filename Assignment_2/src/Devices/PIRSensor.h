#ifndef __PIRSENSOR__
#define __PIRSENSOR__

class PIRSensor {
  
  int pin;

public:
  PIRSensor(int pin);
  bool isMotionDetected();
};

#endif
