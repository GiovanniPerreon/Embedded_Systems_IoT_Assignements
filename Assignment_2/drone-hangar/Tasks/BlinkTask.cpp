#include "BlinkTask.h"

BlinkTask::BlinkTask(int pin){
  this->pin = pin;    
}
  
void BlinkTask::init(int period){
  Task::init(period);
  led = new Led(pin); 
  state = OFF;
}
  
void BlinkTask::tick(){
  switch (state){
    case ON:
      // Stay on, do nothing
      break;
    case OFF:
      // Stay off, do nothing
      break;
    case BLINK_OFF:
      led->switchOn();
      state = BLINK_ON; 
      break;
    case BLINK_ON:
      led->switchOff();
      state = BLINK_OFF;
      break;
  }
}

void BlinkTask::on() {
  led->switchOn();
  state = ON;
}

void BlinkTask::off() {
  led->switchOff();
  state = OFF;
}

void BlinkTask::blink() {
  state = BLINK_OFF;
}
