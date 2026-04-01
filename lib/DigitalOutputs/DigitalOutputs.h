#ifndef DIGITALOUTPUTS
#define DIGITALOUTPUTS

#include "PCF8574.h"
#include "CoilData.h"

class DigitalOutputs {

private:
  bool digitalOutputs[64];

public:
  // Constructors
  DigitalOutputs();
  void RefreshDigitalOutputs(PCF8574& outputsmux1, PCF8574& outputsmux2, CoilData& coils);
  bool Get(unsigned int index);
  void Set(unsigned int index, bool value);
};

#endif