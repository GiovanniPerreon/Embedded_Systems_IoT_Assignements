#include "core.h"
#include "Arduino.h"
#include "Kernel/kernel.h"
#include "Kernel/Scheduler.h"
#include "Tasks/BlinkTask.h"
#include "Tasks/PrintTask.h"
#include "Tasks/ButtonTask.h"
#include "Tasks/ServoTask.h"
#include "Tasks/TempTask.h"
#include "Tasks/UltrasonicTask.h"
#include "Tasks/PIRTask.h"
#include "Tasks/LCDTask.h"
#include "Tasks/DroneHangarTask.h"

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

    Task* t3 = new ServoTask(9);
    t3->init(500);

    Task* t5 = new UltrasonicTask(4, 5); // trig=4, echo=5
    t5->init(1000);

    Task* t6 = new PIRTask(3); // PIR sensor on pin 3
    t6->init(100);

    Task* t7 = new LCDTask(0x27, 16, 2); // LCD 16x2 at 0x27, displays button state
    t7->init(200);

    Task* t4 = new TempTask(A1, *(ButtonTask*)t2, *(ServoTask*)t3, *(BlinkTask*)t0, *(LCDTask*)t7);
    t4->init(2000);

    Task* t8 = new DroneHangarTask(
      *(LCDTask*)t7,
      *(BlinkTask*)t0,
      *(ButtonTask*)t2,
      *(PIRTask*)t6,
      *(ServoTask*)t3,
      *(UltrasonicTask*)t5,
      *(TempTask*)t4
    );
    t8->init(100);

    sched.addTask(t0);
    sched.addTask(t1);
    sched.addTask(t2);
    sched.addTask(t3);
    sched.addTask(t4);
    sched.addTask(t5);
    sched.addTask(t6);
    sched.addTask(t7);
    sched.addTask(t8);
    sched.addTask(t4);
    
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
