#ifndef __CONFIG__
#define __CONFIG__

// #define __DEBUG__

#define LED_PIN 13
#define D1 50
#define D2 50
#define T1 2000
#define T2 2000
#define T3 2000
#define T4 2000
#define TEMP1 30.0
#define TEMP2 35.0
// Serial protocol commands (PC -> Arduino)
#define CMD_TAKEOFF "TAKEOFF"
#define CMD_LAND "LAND"
#define CMD_STATUS "STATUS?"

// Serial protocol responses (Arduino -> PC)
#define RESP_INSIDE "INSIDE"
#define RESP_OUTSIDE "OUTSIDE"
#define RESP_TAKEOFF "TAKEOFF"
#define RESP_LANDING "LANDING"
#define RESP_ALARM "ALARM"
#define RESP_DIST_PREFIX "DIST:"

#endif
