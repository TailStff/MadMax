#include "DigitalOutputs.h"

DigitalOutputs::DigitalOutputs()
{
  for (int i = 0; i < 64; i++)
  {
    digitalOutputs[i] = false;
  }
}

void DigitalOutputs::RefreshDigitalOutputs(PCF8574 &outputsmux1, PCF8574 &outputsmux2, CoilData &coils)
{
  // Inputs from 0 to 7
  for (int i = 0; i <= 7; i++)
  {
    outputsmux1.digitalWrite(i, digitalOutputs[i] ? LOW : HIGH);
  }

  // Inputs from 8 to 15
  for (int i = 0; i <= 7; i++)
  {
    outputsmux2.digitalWrite(i, digitalOutputs[i + 8] ? LOW : HIGH);
  }

  // Replicate Outputs to ModbusServer
  for (int i = 0; i < 64; i++)
  {
    coils.set(i + 100, digitalOutputs[i]);
  }
}

void DigitalOutputs::Set(unsigned int index, bool value)
{
  if (index >= 64)
    return;

  digitalOutputs[index] = value;
}

bool DigitalOutputs::Get(unsigned int index)
{
  if (index >= 64)
    return false;

  return digitalOutputs[index];
}
