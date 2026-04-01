#ifndef MMDELAYONOFF_H
#define MMDELAYONOFF_H

#include <cstdint>
#include "IPrimitive.h"
#include "ExecutionEnv.h"
#include "mmDelayOnOffStatus.h"
#include "structDelayStatus.h"

class mmDelayOnOff : public IPrimitive
{

private:
  int64_t tickNumber;
  uint16_t cycle;

  ExecutionEnv *executionEnv;

  DelayOnOffStatus status;

public:
  // Constructors
  mmDelayOnOff(ExecutionEnv *_executionEnv, bool initialValue = false);
  ~mmDelayOnOff();

  bool Evaluate(bool input, uint32_t delayOn, uint32_t delayOff, DelayStatus *status = nullptr);
  void EmergencyOn();
  void EmergencyOff();

  const int64_t GetRemainingTime() const { return this->status.remainingTime; };
  const bool GetValue() const { return this->status.output; };
  const DelayOnOffStatus GetStatus() const { return this->status; }

  void GetBytesFromData(std::vector<uint8_t> &data) const override;
  void SetDataFromBytes(std::vector<uint8_t> &data) override;
};

#endif