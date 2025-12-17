#include "ServoTask.h"
#include "Arduino.h"

ServoTask::ServoTask(int pin){
  this->pin = pin;    
}
  
void ServoTask::init(int period){
  Task::init(period);
  servo = new ServoMotor(pin);
  servo->on();
  state = AT_0;
}
  
void ServoTask::tick(){
  switch(state) {
    case AT_0:
      for(int i = 0; i < 50; i++) {
        servo->setPosition(0);
      }
      Serial.println("Servo at 0");
      break;
    case AT_180:
      for(int i = 0; i < 50; i++) {
        servo->setPosition(180);
      }
      Serial.println("Servo at 180");
      break;
    }
    //case NOTHING:
      // do nothing
      //break;
}

void ServoTask::open(){
  state = AT_180;
}

void ServoTask::close(){
  state = AT_0;
}
