#include "LCDTask.h"
#include <Arduino.h>

LCDTask::LCDTask(uint8_t addr, uint8_t cols, uint8_t rows)
  : lcd(addr, cols, rows), mode("AUTOMATIC"), valvePercent(0) {}

void LCDTask::init(int period) {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Valve: 0% ");
  lcd.setCursor(0, 1);
  lcd.print("Mode: AUTOMATIC");
}

void LCDTask::tick() {
  lcd.setCursor(0, 0);
  lcd.print("Valve: ");
  lcd.print(valvePercent);
  lcd.print("%   ");
  lcd.setCursor(0, 1);
  lcd.print("Mode: ");
  lcd.print(mode);
  lcd.print("      ");
}

void LCDTask::setMode(const char* m) {
  mode = m;
}

void LCDTask::setValvePercent(int percent) {
  valvePercent = percent;
}
