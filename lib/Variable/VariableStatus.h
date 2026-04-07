#ifndef MADMAXVARIABLESTATUS_H
#define MADMAXVARIABLESTATUS_H

#include <cstdint>

namespace MadMax
{
    template <class T>
    struct VariableStatus
    {
        T value;
    };
}

#endif