#include "LCD.h"
#include "Arduino.h"

LCD::LCD(int address, int cols, int rows){
  this->cols = cols;
  this->rows = rows;
  this->lcd = new LiquidCrystal_I2C(address, cols, rows);
}

void LCD::init(){
  lcd->init();
  lcd->backlight();
}

void LCD::clear(){
  lcd->clear();
}

void LCD::setCursor(int col, int row){
  lcd->setCursor(col, row);
}

void LCD::print(String text){
  lcd->print(text);
}

void LCD::print(int value){
  lcd->print(value);
}

void LCD::print(float value){
  lcd->print(value);
}
