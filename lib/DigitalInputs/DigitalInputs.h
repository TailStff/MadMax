#ifndef DIGITALINPUTS
#define DIGITALINPUTS

#include "PCF8574.h"
#include "CoilData.h"

class DigitalInputs
{
private:
  bool digitalInputs[64];

public:
  // Constructors
  DigitalInputs();
  void RefreshDigitalInputs(PCF8574 &inputsmux1, PCF8574 &inputsmux2, CoilData &coils);
  bool Get(unsigned int index);
};

#endif