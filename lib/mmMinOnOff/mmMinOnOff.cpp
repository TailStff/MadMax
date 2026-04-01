#include "mmMinOnOff.h"

mmMinOnOff::mmMinOnOff(ExecutionEnv *_executionEnv, bool initialValue)
{
  this->executionEnv = _executionEnv;
  this->cycle = executionEnv->GetCycle();
  this->status.minOnTime = 0;
  this->status.minOffTime = 0;
  this->status.output = initialValue;
  this->lastOntickNumber = 0;
  this->lastOfftickNumber = 0;
}

mmMinOnOff::~mmMinOnOff()
{
}

bool mmMinOnOff::Evaluate(bool in, uint32_t minOnTime, uint32_t minOffTime, DelayStatus *status)
{
  this->status.input = in;
  this->status.minOnTime = minOnTime;
  this->status.minOffTime = minOffTime;

  int64_t _tickNumber = executionEnv->GetTicks();

  remainingTime = -1;

  if (!this->status.output && in)
  {
    int64_t elapsedTime = (_tickNumber - lastOfftickNumber) * cycle;

    if (elapsedTime >= minOffTime)
    {
      this->status.output = true;
      lastOntickNumber = _tickNumber;
      remainingTime = -1;
    }
    else
    {
      remainingTime = minOffTime - elapsedTime;
    }
  }
  else if (this->status.output && !in)
  {
    int64_t elapsedTime = (_tickNumber - lastOntickNumber) * cycle;

    if (elapsedTime >= minOnTime)
    {
      this->status.output = false;
      lastOfftickNumber = _tickNumber;
      remainingTime = -1;
    }
    else
    {
      remainingTime = minOnTime - elapsedTime;
    }
  }

  if (status)
  {
    status->remainingTime = remainingTime;
    status->value = this->status.output;
  }

  return this->status.output;
}

void mmMinOnOff::EmergencyOn()
{
  signed long long int _tickNumber = executionEnv->GetTicks();

  this->status.output = true;
  lastOntickNumber = _tickNumber;
}

void mmMinOnOff::EmergencyOff()
{
  signed long long int _tickNumber = executionEnv->GetTicks();

  this->status.output = false;
  lastOfftickNumber = _tickNumber;
}

#pragma region IPersistable
/// @brief Get the bytes vector that represent the object persistency values, here we just serialize all pumps runtimes and start counts in a byte vector
/// @param data Reference to the vector that will receive the bytes that represent the object persistency values
void mmMinOnOff::GetBytesFromData(std::vector<uint8_t> &data) const
{
}

void mmMinOnOff::SetDataFromBytes(std::vector<uint8_t> &data)
{
}
#pragma endregion IPersistable