#ifndef DELAYONOFFSTATUS_H
#define DELAYONOFFSTATUS_H

#include <cstdint>

struct DelayOnOffStatus
{
    bool input;
    bool output;
    uint32_t delayOn;
    uint32_t delayOff;
    int64_t remainingTime;
};

#endif