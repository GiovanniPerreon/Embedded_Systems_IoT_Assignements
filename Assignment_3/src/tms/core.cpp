#include "core.h"
#include "config.h"
#include <Arduino.h>
#include "Kernel/kernel.h"
#include "Kernel/Scheduler.h"
#include "Tasks/BlinkTask.h"
#include "Tasks/UltrasonicTask.h"
#include "Tasks/TmsTask.h"

extern Scheduler sched;

void initCore(){
  Serial.begin(115200);
  sched.init(100);
}

void initState(){
  if (isJustEnteredInState()){
    // Create device tasks
    BlinkTask* greenLedTask = new BlinkTask(GREEN_LED_PIN);
    greenLedTask->init(500);
    BlinkTask* redLedTask = new BlinkTask(RED_LED_PIN);
    redLedTask->init(500);
    UltrasonicTask* ultrasonicTask = new UltrasonicTask(SONAR_TRIG_PIN, SONAR_ECHO_PIN);
    ultrasonicTask->init(500);

    Task* tmsTask = new TmsTask(greenLedTask, redLedTask, ultrasonicTask);
    tmsTask->init(100);
    
    sched.addTask(tmsTask);
    sched.addTask(greenLedTask);
    sched.addTask(redLedTask);
    sched.addTask(ultrasonicTask);
    changeState(RUNNING_STATE);
  }
}

void runningState(){
  if (isJustEnteredInState()){
  }
  sched.schedule();
}
