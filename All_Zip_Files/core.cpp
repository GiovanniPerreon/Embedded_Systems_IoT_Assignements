#include "core.h"
#include "Arduino.h"
#include "Kernel/kernel.h"
#include "Kernel/Scheduler.h"
#include "Tasks/BlinkTask.h"
#include "Tasks/ButtonTask.h"
#include "Tasks/ServoTask.h"
#include "Tasks/TempTask.h"
#include "Tasks/UltrasonicTask.h"
#include "Tasks/PIRTask.h"
#include "Tasks/LCDTask.h"
#include "Tasks/DroneHangarTask.h"

extern Scheduler sched;

/* core logic */

void initCore(){
  Serial.begin(9600);
  sched.init(50);
}

void initState(){
  if (isJustEnteredInState()){
    logMsg("Initializing tasks...");
    
    Task* tL1 = new BlinkTask(L1);
    tL1->init(500);

    Task* tL2 = new BlinkTask(L2);
    tL2->init(500);

    Task* tL3 = new BlinkTask(L3);
    tL3->init(500);

    Task* t1 = new ButtonTask(buttonPIN);
    t1->init(50);

    Task* t2 = new ServoTask(servoPIN);
    t2->init(100);

    Task* t3 = new UltrasonicTask(distanceTrigPIN, distanceEchoPIN);
    t3->init(1000);

    Task* t4 = new PIRTask(PirPIN);
    t4->init(100);

    Task* t5 = new LCDTask(0x27, 16, 2);
    t5->init(200);

    Task* t6 = new TempTask(A1, (ButtonTask*)t1, (ServoTask*)t2, (BlinkTask*)tL3, (LCDTask*)t5);
    t6->init(1000);

    Task* t7 = new DroneHangarTask(
      (LCDTask*)t5,
      (BlinkTask*)tL1,
      (BlinkTask*)tL2,
      (BlinkTask*)tL3,
      (ButtonTask*)t1,
      (PIRTask*)t4,
      (ServoTask*)t2,
      (UltrasonicTask*)t3,
      (TempTask*)t6
    );
    t7->init(100);

    sched.addTask(tL1);
    sched.addTask(tL2);
    sched.addTask(tL3);
    sched.addTask(t1);
    sched.addTask(t2);
    sched.addTask(t6);
    sched.addTask(t3);
    sched.addTask(t4);
    sched.addTask(t5);
    sched.addTask(t7);
    
    logMsg("Tasks initialized. Starting scheduler...");
    changeState(RUNNING_STATE);
  }
}

void runningState(){
  if (isJustEnteredInState()){
    logMsg("Running...");
  }
  sched.schedule();
}
