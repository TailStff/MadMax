#ifndef MMTPULSE_H
#define MMTPULSE_H

#include <stdio.h>
#include <HardwareSerial.h>
#include "IPrimitive.h"
#include "ExecutionEnv.h"
#include "mmTPulseStatus.h"

class mmTPulse : public IPrimitive
{
private:
  // Tick number memorized
  int64_t tickNumber;
  // Cycle duration in ms
  uint16_t cycle;

  // Memorized input (for edge detection)
  bool memIn;

  // Execution Environment Object
  ExecutionEnv *executionEnv;

  TPulseStatus status;

public:
  // Constructors
  mmTPulse(ExecutionEnv *executionEnv, bool initialValue);
  ~mmTPulse();

  bool Evaluate(bool in, int64_t delay);
  void EmergencyOff();

  const bool GetValue() const;
  const TPulseStatus GetStatus() const;

  void GetBytesFromData(std::vector<uint8_t> &data) const override;
  void SetDataFromBytes(std::vector<uint8_t> &data) override;
};

#endif