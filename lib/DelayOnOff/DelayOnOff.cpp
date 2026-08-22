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
    this->status.elapsedTime = 0;

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
        this->status.elapsedTime = delay;
      }
      else
      {
        this->status.remainingTime = delay - elapsed;
        this->status.elapsedTime = elapsed;
      }
    }

    if (status)
    {
      status->input = this->status.input;
      status->delayOff = this->status.delayOff;
      status->delayOn = this->status.delayOn;
      status->remainingTime = this->status.remainingTime;
      status->elapsedTime = this->status.elapsedTime;
      status->output = this->status.output;
    }

    return this->status.output;
  }

  void DelayOnOff::EmergencyOn()
  {
    this->status.output = true;
    this->tickNumber = executionEnv->GetTicks();
    this->status.remainingTime = -1;
    this->status.elapsedTime = 0;
  }

  void DelayOnOff::EmergencyOff()
  {
    this->status.output = false;
    this->tickNumber = executionEnv->GetTicks();
    this->status.remainingTime = -1;
    this->status.elapsedTime = 0;
  }
}