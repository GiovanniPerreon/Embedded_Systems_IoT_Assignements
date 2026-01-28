#include "core.h"
#include "Arduino.h"
#include "kernel.h"
#include "input.h"
#include "config.h"
#include "lcd.h"
#include "arrayHandler.h"
#include <math.h>
#include <LowPower.h>

#define MAX_IDLE_TIME 10000UL

int score = 0;
static int brightness = 0;
static int step = 6;
static unsigned long lastFade = 0;
static const unsigned long FADE_DT = 20;
static int difficulty = 1;
double timeLimit = T1;
int sequenceArr[SQLENGTH];
int answer[SQLENGTH];
int buttonsPressed = 0;
unsigned long inputStartTime = 0;
int timeLeft = 0;

void checkIdleSleep();
void fadeLed();
void updateDifficulty();
void showSequence(int* arr);
void resetStageInput();
void shrinkTimeLimit();
void evaluateAnswer();
void updateCountdown(unsigned long elapsed);

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
  fadeLed();
  updateDifficulty();
  checkIdleSleep();
  if (isButtonPressed(0)) {
    analogWrite(LS_PIN, 0);
    clearLCD();
    lcd.setCursor(0, 0); lcd.print("Go!");
    delay(1000);
    timeLimit = timeLimit / (FACTOR - (getDifficulty() - 1) * ((FACTOR - 0.5) / (MAX_DIFFICULTY - 1)));
    changeState(STAGE1_STATE);
    return;
  }
}

void stage1() {
  if (isJustEnteredInState()) {
    Serial.println("Stage1 (Show Sequence)");
    randomSeed(analogRead(A1));
    generate(sequenceArr);
    showSequence(sequenceArr);
    resetStageInput();
    timeLimit = timeLimit * (FACTOR - (getDifficulty() - 1) * ((FACTOR - 0.5) / (MAX_DIFFICULTY - 1)));
    changeState(STAGE2_STATE);
  }
}

void stage2() {
  if (isJustEnteredInState()) {
    Serial.println("Stage2 (Input)");
  }
  unsigned long elapsed = millis() - inputStartTime;
  if (((int)(timeLimit - elapsed) % 1000) == 0) {
    updateCountdown(elapsed);
  }
  for (int i = 0; i < SQLENGTH; i++) {
    if (isButtonPressed(i) && !isPresent(answer, i + 1)) {
      Serial.println(i + 1);
      resetInput();
      digitalWrite(getLedPin(i), HIGH);
      answer[buttonsPressed] = i + 1;
      buttonsPressed++;
      if (buttonsPressed >= SQLENGTH) {
        changeState(STAGE3_STATE);
        return;
      }
    }
  }
  if (elapsed >= timeLimit) {
    changeState(STAGE3_STATE);
    return;
  }
}

void stage3() {
  if (isJustEnteredInState()) {
    Serial.println("Stage3 (Evaluate)");
  }
  for (int i = 0; i < SQLENGTH; i++) {
    digitalWrite(getLedPin(i), LOW);
  }
  evaluateAnswer();
}

void finalize() {
  if (isJustEnteredInState()) {
    Serial.println("Finalize...");
    score = 0;
    timeLimit = T1;
    resetInput();
    clearLCD();
  }
  changeState(INTRO_STATE);
}

int getDifficulty() {
  return difficulty;
}

void checkIdleSleep() {
    if (getCurrentTimeInState() > MAX_IDLE_TIME) {
        clearLCD();
        lcd.setCursor(0, 0); lcd.print("Sleeping...");
        lcd.setCursor(0, 1); lcd.print("Press B1 to wake");
        Serial.println("Entering deep sleep...");
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
        changeState(INTRO_STATE);
        resetInput();
    }
}

void fadeLed() {
  unsigned long now = millis();
  if (now - lastFade >= FADE_DT) {
    lastFade = now;
    brightness += step;
    if (brightness >= 255 || brightness <= 0) step = -step;
    analogWrite(LS_PIN, brightness);
  }
}

void updateDifficulty() {
  int v = analogRead(POT_PIN);
  difficulty = map(v, 0, 1023, 1, MAX_DIFFICULTY + 1);
  if (difficulty > MAX_DIFFICULTY) difficulty = MAX_DIFFICULTY;
  lcd.setCursor(0, 2);
  lcd.print("Difficulty: ");
  lcd.print(difficulty);
}

void showSequence(int* arr) {
  clearLCD();
  for (int i = 0; i < SQLENGTH; i++) {
    lcd.setCursor(i, 0);
    lcd.print(arr[i]);
  }
  delay(SHOWNTIME);
  clearLCD();
}

void resetStageInput() {
  resetInput();
  memset(answer, 0, sizeof(answer));
  buttonsPressed = 0;
  inputStartTime = millis();
}

void evaluateAnswer() {
    for (int i = 0; i < SQLENGTH; i++) {
        digitalWrite(getLedPin(i), LOW);
    }
    clearLCD();
    if (isEqual(sequenceArr, answer)) {
        score++;

        lcd.setCursor(0, 0);
        lcd.print("GOOD! Score: ");
        lcd.setCursor(14, 0);
        lcd.print(score * 100);
        delay(2000);
        changeState(STAGE1_STATE);
    } else {
        analogWrite(LS_PIN, 255);
        lcd.setCursor(0, 0);
        lcd.print("Game Over!");
        lcd.setCursor(0, 1);
        lcd.print("Final Score: ");
        lcd.setCursor(13, 1);
        lcd.print(score * 100);
        delay(2000);
        analogWrite(LS_PIN, 0);
        delay(8000);
        changeState(FINAL_STATE);
    }
}

void updateCountdown(unsigned long elapsed) {
  int remaining = (timeLimit - elapsed) / 1000;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(remaining);
}
