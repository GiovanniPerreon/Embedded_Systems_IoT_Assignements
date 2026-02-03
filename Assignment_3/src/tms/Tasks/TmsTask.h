#ifndef __TMSTASK_H__
#define __TMSTASK_H__

#include "Task.h"

#include "BlinkTask.h"
#include "UltrasonicTask.h"


// Forward declare PubSubClient
class PubSubClient;

class TmsTask : public Task {
  BlinkTask* greenLedTask;
  BlinkTask* redLedTask;
  UltrasonicTask* ultrasonicTask;
  float lastDistance;
  PubSubClient* mqttClient;
  unsigned long lastMqttPublish;

public:
  TmsTask(BlinkTask* greenLedTask, BlinkTask* redLedTask, UltrasonicTask* ultrasonicTask);
  void init(int period) override;
  void tick() override;
  void setMqttClient(PubSubClient* client);
  void publishLevel();
};

#endif
