#include "TPulse.h"

namespace MadMax
{
  TPulse::TPulse(ExecutionEnv *executionEnv, bool initialValue)
  {
    this->executionEnv = executionEnv;
    this->cycle = executionEnv->GetCycle();

    status = {.input = false, .value = initialValue, .delay = 0};

    // Actual tick value
    tickNumber = 0;

    memIn = false;
  }

  TPulse::~TPulse()
  {
  }

  bool TPulse::Evaluate(bool in, int64_t delay)
  {
    status.input = in;
    status.delay = delay;

    int64_t _tickNumber = executionEnv->GetTicks();

    if (!status.value)
    {
      // we are in low state, we wait for a rising edge on input to start the pulse
      if (!memIn && status.input)
      {
        tickNumber = _tickNumber;
        status.value = true;
      }
    }
    else
    {
      // we are in high state, we wait for the delay to elapse to go back to low state
      int64_t elapsed = (_tickNumber - tickNumber) * cycle;
      if (elapsed >= status.delay)
      {
        status.value = false;
      }
    }

    memIn = status.input;

    return status.value;
  }

  void TPulse::EmergencyOff()
  {
    status.value = false;
    tickNumber = executionEnv->GetTicks();
  }

  const bool TPulse::GetValue() const
  {
    return status.value;
  }

  const TPulseStatus TPulse::GetStatus() const
  {
    return this->status;
  }

  /// @brief Get the bytes vector that represent the object persistency values,
  /// @param data Reference to the vector that will receive the bytes that represent the object persistency values
  void TPulse::GetBytesFromData(std::vector<uint8_t> &data) const
  {
  }

  void TPulse::SetDataFromBytes(std::vector<uint8_t> &data)
  {
  }
}