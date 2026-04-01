#ifndef MADMAXTPULSE_H
#define MADMAXTPULSE_H

#include <cstdio>
#include "IPrimitive.h"
#include "ExecutionEnv.h"
#include "TPulseStatus.h"

namespace MadMax
{
  class TPulse : public IPrimitive
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
    TPulse(ExecutionEnv *executionEnv, bool initialValue);
    ~TPulse();

    bool Evaluate(bool in, int64_t delay);
    void EmergencyOff();

    const bool GetValue() const;
    const TPulseStatus GetStatus() const;

    void GetBytesFromData(std::vector<uint8_t> &data) const override;
    void SetDataFromBytes(std::vector<uint8_t> &data) override;
  };
}

#endif