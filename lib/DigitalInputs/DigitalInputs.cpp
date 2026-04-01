#include "DigitalInputs.h"

DigitalInputs::DigitalInputs()
{
  for (int i = 0; i < 64; i++)
  {
    digitalInputs[i] = false;
  }
}

void DigitalInputs::RefreshDigitalInputs(PCF8574 &inputsmux1, PCF8574 &inputsmux2, CoilData &coils)
{
  // Inputs from 0 to 7
  for (int i = 0; i <= 7; i++)
  {
    digitalInputs[i] = inputsmux1.digitalRead(i) == LOW;
  }
  // Inputs from 8 to 15
  for (int i = 0; i <= 7; i++)
  {
    digitalInputs[i + 8] = inputsmux2.digitalRead(i) == LOW;
  }

  // Replicate Inputs to ModbusServer
  for (int i = 0; i < 64; i++)
  {
    coils.set(i, digitalInputs[i]);
  }
}

bool DigitalInputs::Get(unsigned int index)
{
  if (index >= 64)
    return false;

  return digitalInputs[index];
}