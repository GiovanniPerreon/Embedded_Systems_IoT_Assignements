#ifndef __CONFIG__
#define __CONFIG__

// #define __DEBUG__

#define L1 13
#define L2 12
#define L3 11
#define servoPIN 9
#define distanceEchoPIN 5
#define distanceTrigPIN 4
#define PirPIN 3
#define buttonPIN 2
#define D1 50
#define D2 30
#define T1 3000
#define T2 3000
#define T3 3000
#define T4 3000
#define Temp1 20.0
#define Temp2 21.0
// Serial protocol commands (PC -> Arduino)
#define CMD_TAKEOFF "TAKEOFF"
#define CMD_LAND "LAND"
#define CMD_STATUS "STATUS?"

// Serial protocol responses (Arduino -> PC)
#define RESP_INSIDE "INSIDE"
#define RESP_OUTSIDE "OUTSIDE"
#define RESP_TAKEOFF "TAKEOFF"
#define RESP_LANDING "LANDING"
#define RESP_NORMAL "NORMAL"
#define RESP_PREALARM "PREALARM"
#define RESP_ALARM "ALARM"
#define RESP_DIST_PREFIX "DIST:"

#endif
