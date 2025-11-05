#include "Arduino.h"
#include "lcd.h" 

LiquidCrystal_I2C lcd(0x27, 16, 2);
/* LCD logic*/
void initLCD() {
   lcd.init();
   lcd.backlight();
};

void clearLCD() {
  lcd.clear();
};
