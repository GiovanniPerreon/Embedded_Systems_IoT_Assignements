#include "ButtonTask.h"
#include <Arduino.h>

ButtonTask::ButtonTask(int pin) : pin(pin), lastState(false), pressed(false) {}

void ButtonTask::init(int period) {
  pinMode(pin, INPUT_PULLUP);
}

void ButtonTask::tick() {
  bool state = digitalRead(pin) == LOW;
  if (state && !lastState) {
    pressed = true;
  } else {
    pressed = false;
  }
  lastState = state;
}

bool ButtonTask::isPressed() {
  return pressed;
}
