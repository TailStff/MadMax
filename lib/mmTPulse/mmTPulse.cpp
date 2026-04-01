#include "mmTPulse.h"

mmTPulse::mmTPulse(ExecutionEnv *executionEnv, bool initialValue)
{
  this->executionEnv = executionEnv;
  this->cycle = executionEnv->GetCycle();

  status = {.input = false, .value = initialValue, .delay = 0};

  // Actual tick value
  tickNumber = 0;

  memIn = false;
}

mmTPulse::~mmTPulse()
{
}

bool mmTPulse::Evaluate(bool in, int64_t delay)
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

void mmTPulse::EmergencyOff()
{
  status.value = false;
  tickNumber = executionEnv->GetTicks();
}

const bool mmTPulse::GetValue() const
{
  return status.value;
}

const TPulseStatus mmTPulse::GetStatus() const
{
  return this->status;
}

/// @brief Get the bytes vector that represent the object persistency values,
/// @param data Reference to the vector that will receive the bytes that represent the object persistency values
void mmTPulse::GetBytesFromData(std::vector<uint8_t> &data) const
{
}

void mmTPulse::SetDataFromBytes(std::vector<uint8_t> &data)
{
}