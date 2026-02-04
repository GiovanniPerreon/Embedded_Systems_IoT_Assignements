#ifndef __TEMP_SENSOR_LM35__
#define __TEMP_SENSOR_LM35__

#include "TempSensor.h"

class TempSensorLM35 : public TempSensor {

public:
  TempSensorLM35(int pin);
  float getTemperature() override;

private:
  int pin;
};

#endif
