#include <unity.h>
#include "mmMinOnOff.h"

class FakeExecutionEnv : public ExecutionEnv
{
public:
    int64_t ticks = 0;
    int cycle = 100;

    int64_t GetTicks() override { return ticks; }
    int GetCycle() override { return cycle; }

    void advanceTicks(int64_t dt) { ticks += dt; }
};

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

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_min_off_delay);
    RUN_TEST(test_min_on_delay);
    UNITY_END();
}