#include "mmMinOnOff.h"

mmMinOnOff::mmMinOnOff(ExecutionEnv *_executionEnv, bool initialValue)
{
  this->executionEnv = _executionEnv;
  this->cycle = executionEnv->GetCycle();
  this->minOnTime = 0;
  this->minOffTime = 0;
  this->value = initialValue;
  this->lastOntickNumber = 0;
  this->lastOfftickNumber = 0;
}

mmMinOnOff::~mmMinOnOff()
{
}

bool mmMinOnOff::Evaluate(bool in, unsigned long int minOnTime, unsigned long int minOffTime, DelayStatus *status)
{
  this->minOnTime = minOnTime;
  this->minOffTime = minOffTime;

  int64_t _tickNumber = executionEnv->GetTicks();

  remainingTime = -1;

  if (!value && in)
  {
    int64_t elapsedTime = (_tickNumber - lastOfftickNumber) * cycle;

    if (elapsedTime >= minOffTime)
    {
      value = true;
      lastOntickNumber = _tickNumber;
      remainingTime = -1;
    }
    else
    {
      remainingTime = minOffTime - elapsedTime;
    }
  }
  else if (value && !in)
  {
    int64_t elapsedTime = (_tickNumber - lastOntickNumber) * cycle;

    if (elapsedTime >= minOnTime)
    {
      value = false;
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
    status->value = value;
  }

  return value;
}

void mmMinOnOff::EmergencyOn()
{
  signed long long int _tickNumber = executionEnv->GetTicks();

  value = true;
  lastOntickNumber = _tickNumber;
}

void mmMinOnOff::EmergencyOff()
{
  signed long long int _tickNumber = executionEnv->GetTicks();

  value = false;
  lastOfftickNumber = _tickNumber;
}