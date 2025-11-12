#include "core.h"
#include "Arduino.h"
#include "kernel.h"
#include "input.h"
#include "config.h"
#include "lcd.h"
#include "arrayHandler.h"
#include <math.h>
#include "DeepSleep.h" // Include the new deep sleep module

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

// Core business logic
void initCore() {
  Serial.begin(9600);
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
  difficulty = map(v, 0, 1023, 1, 5);
  if (difficulty > 4) {
    difficulty = 4;
  }
  lcd.setCursor(0, 2);
  lcd.print("Difficulty: ");
  lcd.print(difficulty);
  // ---- Deep sleep after 10 s of inactivity ----
  if (getCurrentTimeInState() > MAX_IDLE_TIME) {
    clearLCD();
    lcd.setCursor(0, 0); lcd.print("Sleeping...");
    lcd.setCursor(0, 1); lcd.print("Press B1 to wake");
    enterDeepSleep();  // Enter deep sleep mode
    delay(50);
    // After wake-up, reset the state
    changeState(INTRO_STATE);
    return;
  }
  // Start on B1 press (preferred: input module flag)
  if (isButtonPressed(0)) {
    analogWrite(LS_PIN, 0);
    clearLCD();
    lcd.setCursor(0, 0); lcd.print("Go!");
    changeState(STAGE1_STATE);
    return;
  }
}

// Stage 1 logic (unchanged)
void stage1() {
  if (isJustEnteredInState()) {
    Serial.println("Stage1...");
    score = 0;
    resetInput();
  }

  if (isButtonPressed(0)) {
    changeState(STAGE2_STATE);
    resetInput();
  }
}

// Stage 2 logic (restored)
void stage2() {
  if (isJustEnteredInState()) {
    Serial.println("Stage2...");
    // Reset the input at the beginning of the stage
    resetInput();
  }

  //while (isAnyButtonPressed()) {
    // Wait for any button to be pressed to start
    // Doesn't actually work as expected?
  //}

  randomSeed(analogRead(A0));

  int sequence[SQLENGTH];
  generate(sequence); // Generate the sequence to be displayed
  
  // Display the sequence on the LCD
  clearLCD();
  lcd.setCursor(0, 0); lcd.print(sequence[0]);
  lcd.setCursor(1, 0); lcd.print(sequence[1]);
  lcd.setCursor(2, 0); lcd.print(sequence[2]);
  lcd.setCursor(3, 0); lcd.print(sequence[3]);

  delay(SHOWNTIME);  // Show the sequence for a short period
  clearLCD();
  resetInput();

  int answer[SQLENGTH] = {0, 0, 0, 0};  // Store the player's answer
  int buttonsPressed = 0;
  timeLimit = timeLimit * (FACTOR - (difficulty - 1) * ((FACTOR - 0.5) / 3));
  int counter = 0;
  int timeLeft;

  // Allow the player to enter their answer
  while (getCurrentTimeInState() - SHOWNTIME < timeLimit && buttonsPressed < SQLENGTH) {
    updateStateTime();
    if (counter > 1000) {
      counter = 0;
      timeLeft = (timeLimit - (getCurrentTimeInState() - SHOWNTIME)) / 1000;
      clearLCD();
      lcd.setCursor(0, 0); lcd.print(timeLeft);  // Display the remaining time
    }
    // Check if any button was pressed
    for (int i = 0; i < SQLENGTH; i++) {
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
  // Reset all LEDs to off
  for (int i = 0; i < SQLENGTH; i++) {
    digitalWrite(getLedPin(i), LOW);
  }
  clearLCD();
  // Check if the player's answer matches the sequence
  if (isEqual(sequence, answer)) {
    score++;
    lcd.setCursor(0, 0); lcd.print("GOOD! Score: ");
    lcd.setCursor(14, 0); lcd.print(score * 100);
    delay(2000);
    changeState(STAGE2_STATE);  // Go back to stage2 if correct
  } else {
    analogWrite(LS_PIN, 255);
    lcd.setCursor(0, 0); lcd.print("Game Over!");
    lcd.setCursor(0, 1); lcd.print("Final Score: ");
    lcd.setCursor(13, 1); lcd.print(score * 100);
    delay(2000);
    analogWrite(LS_PIN, 0);
    delay(8000);
    changeState(STAGE3_STATE);  // Move to stage 3 after game over
    return;
  }
}

// Stage 3 logic (restored)
void stage3() {
  if (isJustEnteredInState()) {
    Serial.println("Stage3...");
    resetInput();
  }
  
  // Change the state if button 1 is pressed or if max time has elapsed
  if (isButtonPressed(2) || getCurrentTimeInState() > MAX_TIME_IN_STAGE3_STATE) {
    changeState(STAGE4_STATE);
  }
}

// Stage 4 logic (restored)
void stage4() {
  if (isJustEnteredInState()) {
    Serial.println("Stage4...");
    resetInput();
  }

  // Change the state if button 1 is pressed or if max time has elapsed
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

// Getter for difficulty (unchanged)
int getDifficulty() {
  return difficulty;
}
