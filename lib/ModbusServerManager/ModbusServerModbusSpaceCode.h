#ifndef MADMAXMODBUSSPACECODE_H
#define MADMAXMODBUSSPACECODE_H

namespace MadMax
{
    enum class ModbusServerModbusSpaceCode
    {
        DiscreteInputs = 1,
        Coils = 2,
        InputRegisters = 3,
        HoldingRegisters = 4
    };
}

#endif