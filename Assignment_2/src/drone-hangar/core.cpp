#include "core.h"
#include "Arduino.h"
#include "kernel.h"
#include "Scheduler.h"
#include "BlinkTask.h"
#include "PrintTask.h"

extern Scheduler sched;

/* core business logic */

void initCore(){
  Serial.begin(9600);
  sched.init(100);
}

void initState(){
  if (isJustEnteredInState()){
    logMsg("Initializing tasks...");
    
    // Create and add tasks here
    Task* t0 = new BlinkTask(13);
    t0->init(100);

    Task* t1 = new PrintTask(t0);
    t1->init(500);
    
    sched.addTask(t0);
    sched.addTask(t1);
    
    logMsg("Tasks initialized. Starting scheduler...");
    changeState(RUNNING_STATE);
  }
}

void runningState(){
  if (isJustEnteredInState()){
    logMsg("Running...");
  }
  
  // Run the scheduler
  sched.schedule();
}
