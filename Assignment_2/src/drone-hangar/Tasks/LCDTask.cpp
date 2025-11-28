#include "LCDTask.h"
#include "Arduino.h"

LCDTask::LCDTask(int address, int cols, int rows, ButtonTask* buttonTask){
  this->lcd = new LCD(address, cols, rows);
  this->buttonTask = buttonTask;
}
  
void LCDTask::init(int period){
  Task::init(period);
  lcd->init();
}
  
void LCDTask::tick(){
  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print("Button: ");
  lcd->setCursor(0, 1);
  
  if (buttonTask->isButtonPressed()){
    lcd->print("PRESSED");
  } else {
    lcd->print("NOT PRESSED");
  }
}
