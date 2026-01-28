#ifndef __LCD__
#define __LCD__
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

/* LCD logic*/
void initLCD();
void clearLCD();

#endif
