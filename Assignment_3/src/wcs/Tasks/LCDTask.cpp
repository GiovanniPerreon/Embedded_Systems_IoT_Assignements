#include "LCDTask.h"
#include "Arduino.h"

LCDTask::LCDTask(int address, int cols, int rows){
  this->lcd = new LCD(address, cols, rows);
}
  
void LCDTask::init(int period){
  Task::init(period);
  lcd->init();
}
  
void LCDTask::tick(){

}

void LCDTask::printLCD(const char* text, int col, int row){
  lcd->setCursor(col, row);
  lcd->print(text);
}

void LCDTask::clear(){
  lcd->clear();
}