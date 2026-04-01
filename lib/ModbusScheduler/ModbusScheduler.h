#pragma once

#include <vector>
#include <functional>
#include <stdint.h>
#include <ModbusClientRTU.h>

enum class MBTaskState
{
    // Task is waiting for its next poll time to be reached, initial state
    Idle,
    Running,
    Done,
    Error,
    Timeout
};

enum class MBFunction
{
    ReadCoils = 1,
    ReadDiscreteInputs = 2,
    ReadHoldingRegisters = 3,
    ReadInputRegisters = 4,
    WriteSingleCoil = 5,
    WriteSingleRegister = 6,
    WriteMultipleCoils = 15,
    WriteMultipleRegisters = 16
};

class MBTask
{
public:
    MBTask();
    ~MBTask();

    void Set(uint8_t slave, MBFunction function, uint16_t address, uint16_t quantity = 1, uint32_t pollIntervalMs = 1000, uint16_t *writeValuePtr = nullptr, std::function<void(const MBTask &)> callback = nullptr);
    void Set2(uint8_t slave, MBFunction function, uint16_t address, uint16_t quantity = 1, uint32_t pollIntervalMs = 1000, uint8_t *writeValuePtr = nullptr, std::function<void(const MBTask &)> callback = nullptr);

    uint8_t slave;
    MBFunction function;
    uint16_t address;
    uint16_t quantity = 1;

    // write value
    uint16_t *writeValuePtr = nullptr;
    uint8_t *coilsValuePtr = nullptr;

    // read result
    std::vector<uint16_t> readData;

    // state
    MBTaskState state = MBTaskState::Idle;

    // scheduling
    uint32_t pollIntervalMs = 1000;
    uint32_t nextPollTime = 0;
    // Flag that can be set when it in case of many pending tasks. Still subject to maxParallel limit.
    bool TriggerRequest = false;

    // communication
    bool inProgress = false;
    uint32_t requestStartTime = 0;
    uint32_t timeoutMs = 1000;

    uint8_t retryCount = 0;
    uint8_t retryMax = 3;

    uint32_t token;

    // callback
    std::function<void(const MBTask &)> callback;

protected:
private:
};

class ModbusScheduler
{
public:
    ModbusScheduler(ModbusClientRTU &client);

    void setMaxParallel(uint8_t n);
    void addTask(const MBTask &task);
    std::vector<MBTask> &getTasks();

    void loop();

private:
    ModbusClientRTU &mb;
    std::vector<MBTask> tasks;
    uint8_t maxParallel = 200;
    uint32_t tokenCounter = 1;
    size_t nextIndex = 0;

    void sendRequest(MBTask &t);
    void handleTimeouts();
    void startPendingTasks();

    void onData(ModbusMessage response, uint32_t token);
    void onError(Modbus::Error error, uint32_t token);

    MBTask *findTaskByToken(uint32_t token);
};
