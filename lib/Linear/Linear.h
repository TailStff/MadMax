#ifndef MADMAXLINEAR_H
#define MADMAXLINEAR_H

#include <stdio.h>
#include <HardwareSerial.h>
#include "ExecutionEnv.h"

namespace MadMax
{
    struct PointXY
    {
        float x;
        float y;
    };

    class Linear
    {
    private:
        ExecutionEnv *executionEnv;

    public:
        // Constructors
        Linear(ExecutionEnv *_executionEnv);
        ~Linear();

        float Evaluate(const PointXY *table, size_t size, float x, bool clamp);
        float Evaluate(const vector<PointXY> table, float x, bool clamp);
    };
}

#endif