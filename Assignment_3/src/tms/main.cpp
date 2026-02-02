
#include "Kernel/kernel.h"
#include "config.h"
#include "Tasks/BlinkTask.h"
#include "Tasks/UltrasonicTask.h"
#include "Tasks/TmsTask.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>

// Task handles
TaskHandle_t ultrasonicTaskHandle = NULL;
TaskHandle_t tmsTaskHandle = NULL;

BlinkTask* greenLedTask;
BlinkTask* redLedTask;
UltrasonicTask* ultrasonicTask;
TmsTask* tmsTask;

void ultrasonicTaskLoop(void* pvParameters) {
  for (;;) {
    ultrasonicTask->tick();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void tmsTaskLoop(void* pvParameters) {
  for (;;) {
    tmsTask->tick();
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  greenLedTask = new BlinkTask(GREEN_LED_PIN);
  greenLedTask->init(200);
  redLedTask = new BlinkTask(RED_LED_PIN);
  redLedTask->init(200);
  ultrasonicTask = new UltrasonicTask(SONAR_TRIG_PIN, SONAR_ECHO_PIN);
  ultrasonicTask->init(200);
  tmsTask = new TmsTask(greenLedTask, redLedTask, ultrasonicTask);
  tmsTask->init(200);

  xTaskCreatePinnedToCore(ultrasonicTaskLoop, "UltrasonicTask", 2048, NULL, 1, &ultrasonicTaskHandle, 0);
  xTaskCreatePinnedToCore(tmsTaskLoop, "TmsTask", 2048, NULL, 1, &tmsTaskHandle, 1);
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
