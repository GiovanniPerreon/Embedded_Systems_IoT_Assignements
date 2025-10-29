#include "core.h"
#include "Arduino.h"
#include "kernel.h"
#include "input.h"

#define MAX_TIME_IN_INTRO_STATE 1000
#define MAX_TIME_IN_STAGE2_STATE 10000
#define MAX_TIME_IN_STAGE3_STATE 10000
#define MAX_TIME_IN_STAGE4_STATE 10000

/* core business logic */

void initCore(){
  Serial.begin(9600);
}

void intro(){
  if (isJustEnteredInState()){
    Serial.println("Intro...");
  }
  
  int dt = getCurrentTimeInState();
  if (dt > MAX_TIME_IN_INTRO_STATE){
    changeState(STAGE1_STATE);
  }
}

void stage1(){
  if (isJustEnteredInState()){
    Serial.println("Stage1...");
    resetInput();
  }

  /* change the state if button 0 is pressed */
  if (isButtonPressed(0)){
    changeState(STAGE2_STATE);          
  }
}

void stage2(){
  if (isJustEnteredInState()){
    Serial.println("Stage2...");
  }
  /* change the state if button 1 is pressed or max time elapsed*/
  if (isButtonPressed(1) || getCurrentTimeInState() > MAX_TIME_IN_STAGE2_STATE){
    changeState(STAGE3_STATE);          
  }
}

void stage3(){
  if (isJustEnteredInState()){
    Serial.println("Stage3...");
  }
  /* change the state if button 1 is pressed or max time elapsed*/
  if (isButtonPressed(2) || getCurrentTimeInState() > MAX_TIME_IN_STAGE3_STATE){
    changeState(STAGE4_STATE);          
  }
}


void stage4(){
  if (isJustEnteredInState()){
    Serial.println("Stage4...");
  }
  /* change the state if button 1 is pressed or max time elapsed*/
  if (isButtonPressed(3) || getCurrentTimeInState() > MAX_TIME_IN_STAGE4_STATE){
    changeState(FINAL_STATE);          
  }
}

void finalize(){
  if (isJustEnteredInState()){
    Serial.println("Finalize...");
  }
  changeState(INTRO_STATE);
}
