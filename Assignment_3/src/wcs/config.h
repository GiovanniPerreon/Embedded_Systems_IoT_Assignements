#ifndef __CONFIG__
#define __CONFIG__

#define SERVO_PIN 9
#ifndef POT_PIN
#define POT_PIN A0 // Analog pin for potentiometer (adjust as needed)
#endif
#define BUTTON_PIN 2
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
#define GREEN_LED_PIN 6
#define RED_LED_PIN 7

#define CMD_MODE_MANUAL "MODE_MANUAL"
#define CMD_MODE_AUTO   "MODE_AUTO"
#define CMD_VALVE_OPEN  "VALVE_OPEN:"
#define CMD_STATUS      "STATUS?"

#define RESP_MODE_MANUAL "MANUAL"
#define RESP_MODE_AUTO   "AUTOMATIC"
#define RESP_UNCONNECTED "UNCONNECTED"
#define RESP_VALVE_OPEN  "VALVE_OPEN:"

#endif
