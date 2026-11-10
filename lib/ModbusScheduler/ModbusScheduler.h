#pragma once

#include <vector>
#include <functional>
#include <ModbusClientRTU.h>
#include "ModbusTask.h"
#include "ModbusRegisters.h"

#include "mmLogger.h"

class ModbusScheduler
{
public:
    /// @brief Constructor for the ModbusScheduler
    /// @param client Reference to the ModbusClientRTU instance used for communication with Modbus devices.
    /// @param readIntervalAfterError Time interval in milliseconds to wait before retrying a task after an error occurs.
    /// @param readIntervalBetweenRetry Time interval in milliseconds to wait before retrying a task after a retryable error occurs.
    ModbusScheduler(ModbusClientRTU &client, uint32_t readIntervalAfterError = 60000, uint32_t readIntervalBetweenRetry = 2000);

    /// @brief Sets the maximum number of tasks that can be executed in parallel.
    /// @param n Maximum number of parallel tasks.
    void setMaxParallel(uint8_t n);

    /// @brief Adds a new Modbus task to the scheduler.
    /// @param task The Modbus task to be added.
    void addTask(const MBTask &task);

    /// @brief Returns a reference to the vector of Modbus tasks managed by the scheduler.
    /// @return Reference to the vector of Modbus tasks.
    std::vector<MBTask> &getTasks();

    /// @brief Main loop of the ModbusScheduler. This method should be called periodically to handle timeouts and start pending tasks.
    void loop();

private:
    ModbusClientRTU &mb;
    std::vector<MBTask> tasks;
    uint8_t maxParallel = 1;
    uint32_t tokenCounter = 1;
    size_t nextIndex = 0;

    uint32_t readIntervalAfterError;
    uint32_t readIntervalBetweenRetry;

    Modbus::Error addModbusRequest(MBTask &t);
    void handleTimeouts();
    void startPendingTasks();

    void onData(ModbusMessage response, uint32_t token);
    void onError(Modbus::Error error, uint32_t token);

    MBTask *findTaskByToken(uint32_t token);
};
