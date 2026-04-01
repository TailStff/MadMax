#pragme once

#include <unity.h>
#include "../FakeExecutionEnv.h"
#include "mmMinOnOff.h"

void test_min_off_delay()
{
    FakeExecutionEnv env;
    mmMinOnOff obj(&env, false);

    DelayStatus status{};

    // demande ON trop tôt
    bool v = obj.Evaluate(true, 0, 1000, &status);
    TEST_ASSERT_FALSE(v);
    TEST_ASSERT_GREATER_THAN(0, status.remainingTime);

    // attendre 1000 ms
    env.advanceTicks(10);

    v = obj.Evaluate(true, 0, 1000, &status);
    TEST_ASSERT_TRUE(v);
    TEST_ASSERT_EQUAL(-1, status.remainingTime);
}

void test_min_on_delay()
{
    FakeExecutionEnv env;
    mmMinOnOff obj(&env, true);
    obj.EmergencyOn();

    DelayStatus status{};

    env.advanceTicks(5);
    bool v = obj.Evaluate(false, 1000, 0, &status);
    TEST_ASSERT_TRUE(v);

    env.advanceTicks(5);
    v = obj.Evaluate(false, 1000, 0, &status);
    TEST_ASSERT_FALSE(v);
}
