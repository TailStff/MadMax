#ifndef MINONOFFSTATUS_H
#define MINONOFFSTATUS_H

#include <cstdint>

struct MinOnOffStatus
{
    bool input;
    bool output;
    uint32_t minOnTime;
    uint32_t minOffTime;
    int64_t remainingTime;
};

#endif