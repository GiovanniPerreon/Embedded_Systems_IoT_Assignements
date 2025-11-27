#include "kernel.h"

/* current state */
int currentState;

/* time in which the system entered in current state */
long enteredStateTime;

/* how long the system is in current state */
long currentTimeInState;

/* flag which is true if it is the first cycle in that state */
bool firstCheckInState; 

/* state management procedures */

int getCurrentState(){
  return currentState;
}

long getCurrentTimeInState(){
  return currentTimeInState;
}

long getEnteredStateTime(){
  return enteredStateTime;
}

bool isJustEnteredInState(){
  bool com = firstCheckInState;
  firstCheckInState = false;
  return com;
}

void changeState(int new_state){
  currentState = new_state;
  enteredStateTime = millis();
  firstCheckInState = true;
}

void updateStateTime(){
  currentTimeInState = millis() - enteredStateTime;
}

void logMsg(const String& msg){
  #ifdef DEBUG
  Serial.println(msg);
  #endif
}
