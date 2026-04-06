#ifndef MADMAXDELAYONOFF_H
#define MADMAXDELAYONOFF_H

#include <cstdint>

#include "IPrimitive.h"
#include "ExecutionEnv.h"
#include "DelayOnOffStatus.h"

namespace MadMax
{
  class DelayOnOff : public IPrimitive
  {
  private:
    int64_t tickNumber;
    uint16_t cycle;

    ExecutionEnv *executionEnv;

    DelayOnOffStatus status;

  public:
    // Constructors
    DelayOnOff(ExecutionEnv *_executionEnv, bool initialValue = false);
    ~DelayOnOff();

    bool Evaluate(bool input, uint32_t delayOn, uint32_t delayOff, DelayOnOffStatus *status = nullptr);
    void EmergencyOn();
    void EmergencyOff();

    const int64_t GetRemainingTime() const { return this->status.remainingTime; };
    const bool GetValue() const { return this->status.output; };
    const DelayOnOffStatus GetStatus() const { return this->status; };
  };
}

#endif