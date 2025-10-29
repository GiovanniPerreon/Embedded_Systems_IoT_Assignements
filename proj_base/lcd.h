#ifndef __LCD__
#define __LCD__
extern LiquidCrystal_I2C lcd;

/* LCD logic*/
void initLCD();
void writeLCD();
void clearLCD();

#endif
