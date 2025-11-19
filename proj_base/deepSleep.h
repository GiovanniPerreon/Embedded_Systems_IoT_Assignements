#ifndef __DEEPSLEEP_H__
#define __DEEPSLEEP_H__

#include <LowPower.h>

void initDeepSleep();
void enterDeepSleep();
void wakeUpFromDeepSleep();
extern volatile bool wokeFlag;

#endif
