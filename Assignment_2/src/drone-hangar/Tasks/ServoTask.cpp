#include "ServoTask.h"
#include "Arduino.h"

ServoTask::ServoTask(int pin){
  this->pin = pin;    
}
  
void ServoTask::init(int period){
  Task::init(period);
  servo = new ServoMotor(pin);
  servo->on();
  state = CLOSED;
  servo->setPosition(0);
}
  
void ServoTask::tick(){
  switch(state) {
    case CLOSED:
      servo->setPosition(0);
      break;
    case OPEN:
      servo->setPosition(180);
      break;
  }
}

void ServoTask::open(){
  state = OPEN;
}

void ServoTask::close(){
  state = CLOSED;
}
