#ifndef __TMSTASK_H__
#define __TMSTASK_H__

#include "Task.h"

#include "BlinkTask.h"
#include "UltrasonicTask.h"


class TmsTask : public Task {
  BlinkTask* greenLedTask;
  BlinkTask* redLedTask;
  UltrasonicTask* ultrasonicTask;
  float lastDistance;

public:
  TmsTask(BlinkTask* greenLedTask, BlinkTask* redLedTask, UltrasonicTask* ultrasonicTask);
  void init(int period) override;
  void tick() override;
};

#endif
