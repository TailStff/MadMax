#pragma once

enum class MBTaskState
{
    // Task is waiting for its next poll time to be reached, initial state
    Idle,
    Running,
    RetryWait,
    Done,
    Failed
};