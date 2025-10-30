#ifndef __KERNEL__
#define __KERNEL__
#define DEBUG

#include "Arduino.h" 

/* general state management procedure */

void initKernel();
void updateStateTime();
void changeState(int new_state);

int getCurrentState();
long getCurrentTimeInState();
long getEnteredStateTime();
bool isJustEnteredInState();

/* for debugging */
void logMsg(const String& msg);
 

#endif
