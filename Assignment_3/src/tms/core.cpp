#include "core.h"
#include "config.h"
#include <Arduino.h>
#include "Kernel/kernel.h"
#include "Kernel/Scheduler.h"
// #include "Tasks/SonarTask.h"
// #include "Tasks/LedTask.h"
// #include "Tasks/MQTTTask.h"

extern Scheduler sched;

void initCore(){
  Serial.begin(115200);
  sched.init(100);
}

void initState(){
  if (isJustEnteredInState()){
    // Task* sonarTask = new SonarTask(SONAR_TRIG_PIN, SONAR_ECHO_PIN);
    // sonarTask->init(100);
    // Task* ledTask = new LedTask(GREEN_LED_PIN, RED_LED_PIN);
    // ledTask->init(100);
    // Task* mqttTask = new MQTTTask();
    // mqttTask->init(200);
    // sched.addTask(sonarTask);
    // sched.addTask(ledTask);
    // sched.addTask(mqttTask);
    changeState(RUNNING_STATE);
  }
}

void runningState(){
  if (isJustEnteredInState()){
  }
  sched.schedule();
}
