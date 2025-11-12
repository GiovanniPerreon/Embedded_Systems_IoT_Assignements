#include "DeepSleep.h"
#include "Arduino.h"

// Initialize the wake flag
volatile bool wokeFlag = false;
/*
// ISR (Interrupt Service Routine) for wake-up
void wakeISR() {
    wokeFlag = true;
}
// Initialize the deep sleep functionality
void initDeepSleep() {
  //attachInterrupt(digitalPinToInterrupt(BUT01_PIN), wakeISR, FALLING); // have to disable the other one if this is needed (doesn't seem so?)
  Serial.println("DeepSleep initialized.");
}
*/
// Function to enter deep sleep mode
void enterDeepSleep() {
  Serial.println("Entering deep sleep...");
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}
/*
// Function to handle wake-up from deep sleep
void wakeUpFromDeepSleep() {
  // Reset the wake flag after waking up
  wokeFlag = false;
  // Detach interrupt after wake-up to prevent multiple wake-ups
  //detachInterrupt(digitalPinToInterrupt(BUT01_PIN));
  Serial.println("Woke up from deep sleep.");
}
*/
