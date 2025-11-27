#include "core.h"
#include "Arduino.h"
#include "Kernel/kernel.h"
#include "Kernel/Scheduler.h"
#include "Tasks/BlinkTask.h"
#include "Tasks/PrintTask.h"
#include "Tasks/ButtonTask.h"

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

    Task* t2 = new ButtonTask(2);
    t2->init(50);

    sched.addTask(t0);
    sched.addTask(t1);
    sched.addTask(t2);
    
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
