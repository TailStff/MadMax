#include <cstdio>

struct ModbusWriteEvent
{
    uint16_t startAddr;
    uint16_t count;
};