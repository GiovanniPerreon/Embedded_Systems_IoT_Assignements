

#include "TmsTask.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include "../config.h"


TmsTask::TmsTask(BlinkTask* greenLedTask, BlinkTask* redLedTask, UltrasonicTask* ultrasonicTask) {
  this->greenLedTask = greenLedTask;
  this->redLedTask = redLedTask;
  this->ultrasonicTask = ultrasonicTask;
  this->mqttClient = nullptr;
  lastDistance = 0;
  lastMqttPublish = 0;
}


void TmsTask::init(int period) {
  Task::init(period);
  greenLedTask->off();
  redLedTask->off();
  lastMqttPublish = 0;
}


void TmsTask::tick() {
  ultrasonicTask->tick();
  float distance = ultrasonicTask->getDistance();
  Serial.print("[TMS] Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Assignment: green ON and red OFF when network/MQTT is OK and sending data, else red ON and green OFF
  bool mqttOk = (mqttClient && mqttClient->connected());
  if (mqttOk) {
    greenLedTask->on();
    redLedTask->off();
  } else {
    greenLedTask->off();
    redLedTask->on();
  }

  // Publish to MQTT at configured interval
  if (mqttOk) {
    unsigned long now = millis();
    if (now - lastMqttPublish > F) {
      publishLevel();
      lastMqttPublish = now;
    }
  }
}

void TmsTask::setMqttClient(PubSubClient* client) {
  mqttClient = client;
}

void TmsTask::publishLevel() {
  if (!mqttClient) return;
  char payload[64];
  float level = ultrasonicTask->getDistance();
  snprintf(payload, sizeof(payload), "{\"level\":%.2f}", level);
  mqttClient->publish(MQTT_TOPIC, payload);
}
