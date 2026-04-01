#pragme once

#include <stdio.h>
#include "ExecutionEnv.h"

class FakeExecutionEnv : public ExecutionEnv
{
public:
    FakeExecutionEnv() : ExecutionEnv(100, nullptr, nullptr) {};

    int64_t ticks = 0;
    uint16_t cycle = 100; // ms

    int64_t GetTicks() override { return ticks; }
    uint16_t GetCycle() override { return cycle; }

    void advanceTicks(int64_t dt) { ticks += dt; }
};