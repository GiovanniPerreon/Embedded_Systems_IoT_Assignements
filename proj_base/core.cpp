#include "core.h"
#include "Arduino.h"
#include "kernel.h"
#include "input.h"
#include "config.h"
#include "lcd.h"
#include "arrayHandler.h"
#include <math.h>

// Automatic deep-sleep mode selector
#ifndef USE_REAL_DEEPSLEEP
  #if defined(ARDUINO_ARCH_AVR) && \
     (defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MEGA2560))
    #define USE_REAL_DEEPSLEEP 1    // Real hardware supports LowPower library
  #else
    #define USE_REAL_DEEPSLEEP 0    // Simulation / unsupported board
  #endif
#endif

#if USE_REAL_DEEPSLEEP
  #include <LowPower.h>
#endif

#define MAX_TIME_IN_INTRO_STATE 1000
#define MAX_TIME_IN_STAGE2_STATE 10000
#define MAX_TIME_IN_STAGE3_STATE 10000
#define MAX_TIME_IN_STAGE4_STATE 10000
#define MAX_IDLE_TIME 10000UL // 10 seconds before nap time

int score = 0;

static int brightness = 0;                 // 0..255 PWM
static int step = 6;                       // fade step
static unsigned long lastFade = 0;
static const unsigned long FADE_DT = 20;   // ~50 Hz
static int difficulty = 1;                 // 1..4
double timeLimit = T1;

// --- Wake ISR for deep sleep (B1) ---
volatile bool wokeFlag = false;
void wakeISR() { wokeFlag = true; }

// ---------------------------------------------------------------------------
// Core business logic
// ---------------------------------------------------------------------------
void initCore() {
  Serial.begin(9600);

#if USE_REAL_DEEPSLEEP
  Serial.println("DeepSleep: REAL (LowPower)");
#else
  Serial.println("DeepSleep: SIMULATED (no LowPower)");
#endif
}

void intro() {
  if (isJustEnteredInState()) {
    Serial.println("Intro...");
    pinMode(LS_PIN, OUTPUT);
    analogWrite(LS_PIN, 0);

    clearLCD();
    lcd.setCursor(0, 0); lcd.print("Welcome to TOS!");
    lcd.setCursor(0, 1); lcd.print("Press B1 to Start");
    resetInput();
  }

  // Pulse red LED LS (non-blocking)
  unsigned long now = millis();
  if (now - lastFade >= FADE_DT) {
    lastFade = now;
    brightness += step;
    if (brightness >= 255 || brightness <= 0) step = -step;
    analogWrite(LS_PIN, brightness);
  }

  // Read difficulty from potentiometer
  int v = analogRead(POT_PIN);
  difficulty = map(v, 0, 1022, 1, 4);
  lcd.setCursor(0, 1);  // LCD has only 2 rows (0 and 1)
  lcd.print("Press B1 | L:");
  lcd.print(difficulty);
  lcd.print("  ");

  // ---- Deep sleep after 10 s of inactivity ----
  if (getCurrentTimeInState() > MAX_IDLE_TIME) {
    clearLCD();
    lcd.setCursor(0, 0); lcd.print("Sleeping...");
    lcd.setCursor(0, 1); lcd.print("Press B1 to wake");
    delay(500); // brief pause so the message is visible

  #if USE_REAL_DEEPSLEEP
    // Arm interrupt on B1 (BUT01_PIN should be an INT-capable pin, e.g., D2 on UNO)
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    wokeFlag = false;
  #else
    // Tinkercad fallback: simulate a nap
    delay(1500);
  #endif

    // On wake/simulated wake, restart intro fresh
    changeState(INTRO_STATE);
    return;
  }

  // Start on B1 press (preferred: input module flag)
  if (isButtonPressed(0)) {
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

void stage1() {
  if (isJustEnteredInState()) {
    Serial.println("Stage1...");
    resetInput();
  }

  /* change the state if button 0 is pressed */
  if (isButtonPressed(0)) {
    changeState(STAGE2_STATE);
    resetInput();
  }
}

void stage2() {
  if (isJustEnteredInState()) {
    Serial.println("Stage2...");
    resetInput();
  }

  while (isAnyButtonPressed()) {
    
  }

  randomSeed(analogRead(A0));

  for (int i; i < SQLENGTH; i++) {
    digitalWrite(getLedPin(i), LOW);
  }

  int sequence[SQLENGTH];
  generate(sequence);
  
  clearLCD();
  lcd.setCursor(0, 0); lcd.print(sequence[0]);
  lcd.setCursor(1, 0); lcd.print(sequence[1]);
  lcd.setCursor(2, 0); lcd.print(sequence[2]);
  lcd.setCursor(3, 0); lcd.print(sequence[3]);
  
  delay(SHOWNTIME);

  clearLCD();

  resetInput();
  
  int answer[SQLENGTH] = {0, 0, 0, 0};
  int buttonsPressed = 0;
  timeLimit = timeLimit * (FACTOR - ((difficulty - 1) / 5));
  int counter = 0;
  int timeLeft;

  while (getCurrentTimeInState() - SHOWNTIME < timeLimit && buttonsPressed < SQLENGTH) {
    updateStateTime();
    if (counter > 1000) {
      counter = 0;
      timeLeft = (timeLimit - (getCurrentTimeInState() - SHOWNTIME)) / 1000;
      clearLCD();
      lcd.setCursor(0, 0); lcd.print(timeLeft);
    }
    for (int i = 0; i <= SQLENGTH - 1; i++) {
      if (isButtonPressed(i) && !isPresent(answer, i + 1)) {
        Serial.println(i + 1);
        resetInput();
        digitalWrite(getLedPin(i), HIGH);
        answer[buttonsPressed] = i + 1;
        buttonsPressed++;
      }
    }
    counter++;
  }
  
  clearLCD();
  
  if (isEqual(sequence, answer)) {
    score++;
    lcd.setCursor(0, 0); lcd.print("GOOD! Score: ");
    lcd.setCursor(14, 0); lcd.print(score*100);
    delay(2000);
    changeState(STAGE2_STATE);
  } else {
    lcd.setCursor(0, 0); lcd.print("Game Over!");
    lcd.setCursor(0, 1); lcd.print("Final Score: ");
    lcd.setCursor(13, 1); lcd.print(score*100);
    changeState(STAGE3_STATE);
    return;
  }
}

void stage3() {
  if (isJustEnteredInState()) {
    Serial.println("Stage3...");
    resetInput();
  }
  /* change the state if button 1 is pressed or max time elapsed */
  if (isButtonPressed(2) || getCurrentTimeInState() > MAX_TIME_IN_STAGE3_STATE) {
    changeState(STAGE4_STATE);
  }
}

void stage4() {
  if (isJustEnteredInState()) {
    Serial.println("Stage4...");
    resetInput();
  }
  /* change the state if button 1 is pressed or max time elapsed */
  if (isButtonPressed(3) || getCurrentTimeInState() > MAX_TIME_IN_STAGE4_STATE) {
    changeState(FINAL_STATE);
  }
}

void finalize() {
  if (isJustEnteredInState()) {
    Serial.println("Finalize...");
    resetInput();
  }
  changeState(INTRO_STATE);
}

// simple getter for difficulty
int getDifficulty() {
  return difficulty;
}
