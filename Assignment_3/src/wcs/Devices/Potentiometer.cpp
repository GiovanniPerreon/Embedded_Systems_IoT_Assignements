#include "Potentiometer.h"

Potentiometer::Potentiometer(int pin) : pin(pin) {}

void Potentiometer::init() {
  pinMode(pin, INPUT);
}

int Potentiometer::readValue() {
  return analogRead(pin);
}
