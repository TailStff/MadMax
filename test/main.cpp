#include "FakeExecutionEnv.h"
#include "test_mmMinOnOff.h"

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_min_off_delay);
    RUN_TEST(test_min_on_delay);
    UNITY_END();
}