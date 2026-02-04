#include "ButtonTask.h"
#include "Arduino.h"

ButtonTask::ButtonTask(int pin){
  this->pin = pin;    
}
  
void ButtonTask::init(int period){
  Task::init(period);
  button = new Button(pin);
  state = NOT_PRESSED;
  pressedEventFlag = false;
}
  
void ButtonTask::tick(){
  if(button->isPressed() && state == NOT_PRESSED) {
    Serial.println("Button is pressed");
    state = PRESSED;
    pressedEventFlag = true;  // Set flag on press event
  } else if (!button->isPressed() && state == PRESSED){
    Serial.println("Button is not pressed");
    state = NOT_PRESSED;
  }
}

bool ButtonTask::isButtonPressed(){
  if (pressedEventFlag) {
    pressedEventFlag = false;  // Clear flag after reading
    return true;
  }
  return false;
}
