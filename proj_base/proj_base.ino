#include "kernel.h"
#include "config.h"
#include "core.h"
#include "input.h" 
#include "lcd.h"

void setup() {
  initCore();
  initLCD();
  initInput();
  changeState(INTRO_STATE);
}

void loop(){ 
  updateStateTime(); 
  switch (getCurrentState()) { 
  case INTRO_STATE:
    intro();
    break;
  case STAGE1_STATE:
    stage1();
    break;
  case STAGE2_STATE:
    stage2();
    break;
  case STAGE3_STATE:
    stage3();
    break;
  case FINAL_STATE:
    finalize();
    break;
  }
}
