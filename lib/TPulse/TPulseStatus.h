#ifndef MADMAXTPULSESTATUS_H
#define MADMAXTPULSESTATUS_H

#include <cstdio>

namespace MadMax
{
    struct TPulseStatus
    {
        bool input;
        bool value;
        int64_t delay;
    };
}

#endif