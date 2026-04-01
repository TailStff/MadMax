#ifndef MADMAXMINISCHEDULER_H
#define MADMAXMINISCHEDULER_H

#include <stdio.h>
#include "ExecutionEnv.h"

#define ANY -1

namespace MadMax
{
    struct DateTimeDefinition
    {
        int16_t Year;
        int8_t Month;
        int8_t Day;
        int8_t DayOfWeek;
        int8_t Hour;
        int8_t Minute;
        int8_t Second;
    };

    class MiniScheduler
    {
    private:
        ExecutionEnv *executionEnv;

        DateTime lastExecution;
        bool memMatch;

    public:
        // Constructors
        MiniScheduler(ExecutionEnv *_executionEnv);
        ~MiniScheduler();

        bool Evaluate(DateTimeDefinition &dateTime);
    };
}

#endif