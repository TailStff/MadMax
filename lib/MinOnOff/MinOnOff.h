#pragma once

#ifndef MADMAXMINONOFF
#define MADMAXMINONOFF

#include <cstdint>

#include "IPrimitive.h"
#include "ExecutionEnv.h"
#include "MinOnOffStatus.h"

namespace MadMax
{
  class MinOnOff : public IPrimitive
  {

  private:
    ExecutionEnv *executionEnv;

    MinOnOffStatus status;

    signed long long int lastOntickNumber, lastOfftickNumber;
    unsigned int cycle;

  public:
    // Constructors
    MinOnOff(ExecutionEnv *_executionEnv, bool initialValue);
    ~MinOnOff();

    bool Evaluate(bool in, uint32_t minOnTime, uint32_t minOffTime, MinOnOffStatus *status = nullptr);
    void EmergencyOn();
    void EmergencyOff();

    const bool GetValue() const { return this->status.output; };
    const MinOnOffStatus GetStatus() const { return this->status; }
  };
}

#endif