#include "Kernel/kernel.h"
#include "config.h"
#include "core.h"
#include "Kernel/Scheduler.h"

Scheduler sched;

void setup() {
  initCore();
  changeState(INIT_STATE);
  Serial.begin(115200);
}

void loop(){ 
  updateStateTime(); 
  switch (getCurrentState()) { 
  case INIT_STATE:
    initState();
    break;
  case RUNNING_STATE:
    runningState();
    break;
  }
}
