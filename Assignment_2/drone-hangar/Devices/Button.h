#ifndef __BUTTONIMPL__
#define __BUTTONIMPL__

class Button {
 
public: 
  Button(int pin);
  bool isPressed();

private:
  int pin;

};

#endif
