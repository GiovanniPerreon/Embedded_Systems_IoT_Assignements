#include "core.h"
#include "Arduino.h"
#include "kernel.h"
#include "input.h"
#include "config.h"
#include "lcd.h"

#define MAX_TIME_IN_INTRO_STATE 1000
#define MAX_TIME_IN_STAGE2_STATE 10000
#define MAX_TIME_IN_STAGE3_STATE 10000
#define MAX_TIME_IN_STAGE4_STATE 10000
#define MAX_IDLE_TIME 10000UL //10 seconds before nap time

// Local static vars just for the intro screen
static int brightness = 0;                 // 0..255 PWM
static int step = 6;                       // fade step
static unsigned long lastFade = 0;
static const unsigned long FADE_DT = 20;   // ~50 Hz
static int difficulty = 1;                 // 1..4

/* core business logic */

void initCore(){
  Serial.begin(9600);
}

void intro() {
  if (isJustEnteredInState()) {
    Serial.println("Intro...");
    pinMode(LS_PIN, OUTPUT);          // LS_PIN defined in config.h
    analogWrite(LS_PIN, 0);

    clearLCD();
    lcd.setCursor(0, 0); lcd.print("Welcome to TOS!");
    lcd.setCursor(0, 1); lcd.print("Press B1 to Start");
  }

  //Pulse red LED LS (non-blocking)
  unsigned long now = millis();
  if (now - lastFade >= FADE_DT) {
    lastFade = now;
    brightness += step;
    if (brightness >= 255 || brightness <= 0) step = -step;
    analogWrite(LS_PIN, brightness);
  }

  // Read difficulty from potentiometer
  int v = analogRead(POT_PIN);
  difficulty = 1 + (v * 4) / 1024;
  if (difficulty < 1) difficulty = 1;
  if (difficulty > 4) difficulty = 4;

  //Simulated “sleep” after 10 s of inactivity
  if (getCurrentTimeInState() > MAX_IDLE_TIME) {
    clearLCD();
    lcd.setCursor(0, 0); lcd.print("Sleeping...");
    lcd.setCursor(0, 1); lcd.print("Press B1 to wake");
    delay(1500);                        // simulate sleep; real HW => LowPower.powerDown(...)
    changeState(INTRO_STATE);           // wake returns to idle
    return;
  }

  // Start on B1 press
  // Normal path (real HW with interrupts): use the input module flag
  if (isButtonPressed(0)) {             // button index 0 == B1
    clearLCD();
    lcd.setCursor(0, 0); lcd.print("Go!");
    delay(700);
    changeState(STAGE1_STATE);
    return;
  }

  // Fallback for Tinkercad (no interrupts): poll the pin directly
  if (digitalRead(BUT01_PIN) == HIGH) { // defined in config.h
    delay(20);                          // simple debounce
    if (digitalRead(BUT01_PIN) == HIGH) {
      clearLCD();
      lcd.setCursor(0, 0); lcd.print("Go!");
      delay(700);
      changeState(STAGE1_STATE);
      return;
    }
  }
}

void stage1(){
  if (isJustEnteredInState()){
    Serial.println("Stage1...");
    resetInput();
  }

  /* change the state if button 0 is pressed */
  if (isButtonPressed(0)){
    changeState(STAGE2_STATE);          
  }
}

void stage2(){
  if (isJustEnteredInState()){
    Serial.println("Stage2...");
  }
  /* change the state if button 1 is pressed or max time elapsed*/
  if (isButtonPressed(1) || getCurrentTimeInState() > MAX_TIME_IN_STAGE2_STATE){
    changeState(STAGE3_STATE);          
  }
}

void stage3(){
  if (isJustEnteredInState()){
    Serial.println("Stage3...");
  }
  /* change the state if button 1 is pressed or max time elapsed*/
  if (isButtonPressed(2) || getCurrentTimeInState() > MAX_TIME_IN_STAGE3_STATE){
    changeState(STAGE4_STATE);          
  }
}


void stage4(){
  if (isJustEnteredInState()){
    Serial.println("Stage4...");
  }
  /* change the state if button 1 is pressed or max time elapsed*/
  if (isButtonPressed(3) || getCurrentTimeInState() > MAX_TIME_IN_STAGE4_STATE){
    changeState(FINAL_STATE);          
  }
}

void finalize(){
  if (isJustEnteredInState()){
    Serial.println("Finalize...");
  }
  changeState(INTRO_STATE);
}

//simple getter for difficulty
int getDifficulty() { 
  return difficulty; 
}
