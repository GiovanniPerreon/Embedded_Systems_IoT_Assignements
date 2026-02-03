
#include "Kernel/kernel.h"
#include "config.h"
#include "Tasks/BlinkTask.h"
#include "Tasks/UltrasonicTask.h"
#include "Tasks/TmsTask.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

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

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMqtt() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("TMSClient")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  setupWiFi();
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  reconnectMqtt();

  greenLedTask = new BlinkTask(GREEN_LED_PIN);
  greenLedTask->init(200);
  redLedTask = new BlinkTask(RED_LED_PIN);
  redLedTask->init(200);
  ultrasonicTask = new UltrasonicTask(SONAR_TRIG_PIN, SONAR_ECHO_PIN);
  ultrasonicTask->init(200);
  tmsTask = new TmsTask(greenLedTask, redLedTask, ultrasonicTask);
  tmsTask->init(200);
  tmsTask->setMqttClient(&mqttClient);

  xTaskCreatePinnedToCore(ultrasonicTaskLoop, "UltrasonicTask", 2048, NULL, 1, &ultrasonicTaskHandle, 0);
  xTaskCreatePinnedToCore(tmsTaskLoop, "TmsTask", 2048, NULL, 1, &tmsTaskHandle, 1);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMqtt();
  }
  mqttClient.loop();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
