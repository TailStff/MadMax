#ifndef MADMAXDIGITALEQUIPMENTSTATUS_H
#define MADMAXDIGITALEQUIPMENTSTATUS_H

#include <stdio.h>

namespace MadMax
{
    struct DigitalEquipmentStatus
    {
        bool command;
        bool feedback;
        bool fault;
        bool feedbackFault;
        bool output;
        uint64_t runTimeValue;
        uint64_t startCountValue;
    };
}

#endif