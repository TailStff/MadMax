#ifndef MMLINEAR_H
#define MMLINEAR_H

#include <stdio.h>
#include <HardwareSerial.h>
#include "ExecutionEnv.h"

struct PointXY
{
    float x;
    float y;
};

class mmLinear
{
private:
    ExecutionEnv *executionEnv;

public:
    // Constructors
    mmLinear(ExecutionEnv *_executionEnv);
    ~mmLinear();

    float Evaluate(const PointXY *table, size_t size, float x, bool clamp);
};

#endif