#include "DelayOnOff.h"

namespace MadMax
{
  DelayOnOff::DelayOnOff(ExecutionEnv *executionEnv, bool initialValue)
  {
    this->executionEnv = executionEnv;
    this->cycle = executionEnv->GetCycle();
    this->status.delayOn = 0;
    this->status.delayOff = 0;

    // Ouput value of the bloc
    this->status.output = initialValue;

    // Actual tick value
    tickNumber = executionEnv->GetTicks();
  }

  DelayOnOff::~DelayOnOff()
  {
  }

  bool DelayOnOff::Evaluate(bool input, uint32_t delayOn, uint32_t delayOff, DelayOnOffStatus *status)
  {
    this->status.input = input;
    this->status.delayOn = delayOn;
    this->status.delayOff = delayOff;

    int64_t actualTickNumber = executionEnv->GetTicks();

    this->status.remainingTime = -1;

    if (input == this->status.output)
    {
      tickNumber = actualTickNumber;
    }
    else
    {
      uint32_t delay = input ? this->status.delayOn : this->status.delayOff;
      uint32_t elapsed = (actualTickNumber - tickNumber) * cycle;

      if (elapsed >= delay)
      {
        this->status.output = input;
        tickNumber = actualTickNumber;
        this->status.remainingTime = 0;
      }
      else
      {
        this->status.remainingTime = delay - elapsed;
      }
    }

    if (status)
    {
      status->input = this->status.input;
      status->delayOff = this->status.delayOff;
      status->delayOn = this->status.delayOn;
      status->remainingTime = this->status.remainingTime;
      status->output = this->status.output;
    }

    return this->status.output;
  }

  void DelayOnOff::EmergencyOn()
  {
    this->status.output = true;
    this->tickNumber = executionEnv->GetTicks();
    this->status.remainingTime = -1;
  }

  void DelayOnOff::EmergencyOff()
  {
    this->status.output = false;
    this->tickNumber = executionEnv->GetTicks();
    this->status.remainingTime = -1;
  }

#pragma region IPersistable
  /// @brief Get the bytes vector that represent the object persistency values, here we just serialize all pumps runtimes and start counts in a byte vector
  /// @param data Reference to the vector that will receive the bytes that represent the object persistency values
  void DelayOnOff::GetBytesFromData(std::vector<uint8_t> &data) const
  {
  }

  void DelayOnOff::SetDataFromBytes(std::vector<uint8_t> &data)
  {
  }
#pragma endregion IPersistable
}