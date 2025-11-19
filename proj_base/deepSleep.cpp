#include "DeepSleep.h"
#include "Arduino.h"

volatile bool wokeFlag = false;
void enterDeepSleep() {
  Serial.println("Entering deep sleep...");
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}
