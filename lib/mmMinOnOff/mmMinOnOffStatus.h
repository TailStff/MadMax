#ifndef MADMAXMINONOFFSTATUS_H
#define MADMAXMINONOFFSTATUS_H

#include <cstdint>

namespace MadMax
{
    struct MinOnOffStatus
    {
        bool input;
        bool output;
        uint32_t minOnTime;
        uint32_t minOffTime;
        int64_t remainingTime;
    };
}

#endif