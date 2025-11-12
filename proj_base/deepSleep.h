#ifndef DEEPSLEEP_H
#define DEEPSLEEP_H

// Include necessary libraries
#include <LowPower.h>

// Function to initialize deep sleep
void initDeepSleep();

// Function to enter deep sleep mode
void enterDeepSleep();

// Function to handle waking up from deep sleep
void wakeUpFromDeepSleep();

// Flag to indicate if the device has woken up from deep sleep
extern volatile bool wokeFlag;

#endif // DEEPSLEEP_H
