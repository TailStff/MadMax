#ifndef TPULSESTATUS_H
#define TPULSESTATUS_H

#include <stdio.h>

struct TPulseStatus
{
    bool input;
    bool value;
    int64_t delay;
};

#endif