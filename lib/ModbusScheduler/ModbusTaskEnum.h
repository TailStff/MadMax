#pragma once

enum class MBTaskState
{
    // Task is waiting for its next poll time to be reached, initial state
    Idle,
    // Task is currently being processed by the Modbus client
    Running,
    // Task has encountered an error and is waiting for the next retry interval
    RetryWait,
    // Task has completed successfully
    Done,
    // Task has failed after exhausting all retries
    Failed
};