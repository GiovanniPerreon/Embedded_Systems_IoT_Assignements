#include "input.h"
#include "Arduino.h"
#include "config.h"

#include <EnableInterrupt.h>

#define BOUNCING_TIME 50

// #define __DEBUG__

uint8_t inputPins[NUM_BUTTONS] = {BUT01_PIN, BUT02_PIN, BUT03_PIN, BUT04_PIN};
uint8_t outputPins[NUM_LED] = {LED1, LED2, LED3, LED4};

bool buttonPressed[NUM_BUTTONS] = {false, false, false, false};

/* used for debouncing */
long lastButtonPressedTimestamps[NUM_BUTTONS];

void buttonHandler(int i);
void buttonHandler0(){ buttonHandler(0); }
void buttonHandler1(){ buttonHandler(1); }
void buttonHandler2(){ buttonHandler(2); }
void buttonHandler3(){ buttonHandler(3); }

void (*buttonHandlers[NUM_BUTTONS])() = { buttonHandler0, buttonHandler1, buttonHandler2, buttonHandler3};

void buttonHandler(int i){
  long ts = millis();
  if (ts - lastButtonPressedTimestamps[i] > BOUNCING_TIME){
    lastButtonPressedTimestamps[i] = ts;
    int status = digitalRead(inputPins[i]);
    if (status == HIGH && !buttonPressed[i]) { 
        buttonPressed[i] = true;
    }
  }
}

bool isAnyButtonPressed() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (isButtonPressed(i)) {
      Serial.println("true;");
      return true;
    }
  }
  Serial.println("not true;");
  return false;
}

void initInput(){
  for (int i = 0; i < NUM_LED; i++) {
    pinMode(outputPins[i], OUTPUT);      
  }
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(inputPins[i], INPUT);  
    enableInterrupt(inputPins[i], buttonHandlers[i], CHANGE);       
  }
}

void resetInput(){
  long ts = millis();
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttonPressed[i] = false;      
    lastButtonPressedTimestamps[i] = ts;    
  }
}
bool isButtonPressed(int buttonIndex){
  return buttonPressed[buttonIndex];
}

bool hasBeenPressed(int index){
  return buttonPressed[index]; 
}

int getLedPin(int index) {
  return outputPins[index];
}
