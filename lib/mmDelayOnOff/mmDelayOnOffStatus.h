#ifndef DELAYONOFFSTATUS_H
#define DELAYONOFFSTATUS_H

#include <cstdint>

namespace MadMax
{
    struct DelayOnOffStatus
    {
        bool input;
        bool output;
        uint32_t delayOn;
        uint32_t delayOff;
        int64_t remainingTime;
    };
}

#endif