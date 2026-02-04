#include "core.h"
#include "config.h"
#include "Arduino.h"
#include "Kernel/kernel.h"
#include "Kernel/Scheduler.h"
#include "Tasks/ButtonTask.h"
#include "Tasks/ServoTask.h"
#include "Tasks/LCDTask.h"
#include "Tasks/WcsTask.h"

extern Scheduler sched;

void initCore(){
  Serial.begin(9600);
  sched.init(100);
}

void initState(){
  if (isJustEnteredInState()){
    // Initialize tasks for WCS
    ButtonTask* buttonTask = new ButtonTask(BUTTON_PIN);
    buttonTask->init(100);
    ServoTask* servoTask = new ServoTask(SERVO_PIN);
    servoTask->init(100);
    LCDTask* lcdTask = new LCDTask(LCD_ADDR, LCD_COLS, LCD_ROWS);
    lcdTask->init(200);
    // Potentiometer pin for manual mode (define POT_PIN in config.h)
    int potPin = POT_PIN;
    WcsTask* wcsTask = new WcsTask(lcdTask, servoTask, buttonTask, potPin);
    wcsTask->init(100);
    sched.addTask(buttonTask);
    sched.addTask(servoTask);
    sched.addTask(lcdTask);
    sched.addTask(wcsTask);
    changeState(RUNNING_STATE);
  }
}

void runningState(){
  if (isJustEnteredInState()){
    // Optionally log
  }
  sched.schedule();
}
