#ifndef MINONOFF
#define MINONOFF

#include <stdio.h>
#include <HardwareSerial.h>
#include "ExecutionEnv.h"
#include "structDelayStatus.h"

class mmMinOnOff
{

private:
  signed long long int lastOntickNumber, lastOfftickNumber;
  unsigned int cycle;
  unsigned long int minOnTime, minOffTime;
  bool value;
  ExecutionEnv *executionEnv;
  int64_t remainingTime;

public:
  // Constructors
  mmMinOnOff(ExecutionEnv *_executionEnv, bool initialValue);
  ~mmMinOnOff();

  bool Evaluate(bool in, unsigned long int minOnTime, unsigned long int minOffTime, DelayStatus *status = nullptr);
  void EmergencyOn();
  void EmergencyOff();
};

#endif