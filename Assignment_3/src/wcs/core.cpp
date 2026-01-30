#include "core.h"
#include "Arduino.h"
#include "Kernel/kernel.h"
#include "Kernel/Scheduler.h"
#include "Tasks/ButtonTask.h"
#include "Tasks/ServoTask.h"
#include "Tasks/LCDTask.h"

extern Scheduler sched;

void initCore(){
  Serial.begin(9600);
  sched.init(100);
}

void initState(){
  if (isJustEnteredInState()){
    // Initialize tasks for WCS
    Task* buttonTask = new ButtonTask(BUTTON_PIN);
    buttonTask->init(50);
    Task* servoTask = new ServoTask(SERVO_PIN, POT_PIN);
    servoTask->init(100);
    Task* lcdTask = new LCDTask(LCD_ADDR, LCD_COLS, LCD_ROWS);
    lcdTask->init(200);
    sched.addTask(buttonTask);
    sched.addTask(servoTask);
    sched.addTask(lcdTask);
    changeState(RUNNING_STATE);
  }
}

void runningState(){
  if (isJustEnteredInState()){
    // Optionally log
  }
  sched.schedule();
}
