#ifndef __KERNEL__
#define __KERNEL__

#include <Arduino.h>

#define DEBUG

void initKernel();
void updateStateTime();
void changeState(int new_state);

int getCurrentState();
long getCurrentTimeInState();
long getEnteredStateTime();
bool isJustEnteredInState();

void logMsg(const String& msg);

#endif
