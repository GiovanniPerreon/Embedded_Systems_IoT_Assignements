#include "kernel.h"
#include "config.h"
#include "core.h"
#include "Scheduler.h"

Scheduler sched;

void setup() {
  initCore();
  changeState(INIT_STATE);
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
