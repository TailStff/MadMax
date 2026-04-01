#include <stdio.h>
#include "ExecutionEnv.h"

class FakeExecutionEnv : public ExecutionEnv
{
public:
    int64_t ticks = 0;
    int cycle = 100; // ms

    int64_t GetTicks() override { return ticks; }
    int GetCycle() override { return cycle; }

    void advanceTicks(int64_t dt) { ticks += dt; }
};