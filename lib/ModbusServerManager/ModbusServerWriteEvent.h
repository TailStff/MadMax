#include <cstdio>

#include "ModbusServerModbusSpaceCode.h"

struct ModbusWriteEvent
{
    MadMax::ModbusServerModbusSpaceCode modbusSpace;
    uint16_t startAddr;
    uint16_t count;
};