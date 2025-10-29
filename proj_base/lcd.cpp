#include "Arduino.h"
#include <LiquidCrystal_I2C.h> 

/* LCD logic*/
void initLCD() {
   lcd.init();
   lcd.backlight();
};

void writeLCD() {
  clearLCD();
  lcd.setCursor(2, 1);
  lcd.print("Hello World!");
};

void clearLCD() {
  lcd.clear();
};
