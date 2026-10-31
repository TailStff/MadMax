#pragma once

#include <vector>
#include <functional>
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

    /// @brief Set a new task with the specified parameters. This method initializes the task's properties, including the slave address, function code, starting address, quantity of registers or coils to read/write, polling interval, pointer to the value(s) to write (if applicable), and a callback function to be invoked upon task completion or error.
    /// @param slave Slave address of the Modbus device to communicate with.
    /// @param function Function code representing the Modbus operation to perform (e.g., read/write).
    /// @param address Starting address of the Modbus operation.
    /// @param quantity Quantity of registers or coils to read/write.
    /// @param pollIntervalMs Polling interval in milliseconds.
    /// @param writeValuePtr Pointer to the value(s) to send through the Modbus interface (nullptr if not applicable).
    /// @param callback Callback function to be invoked upon task completion or error.
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

    // Flag that indicates whether the task should be triggered immediately, bypassing the normal polling interval.
    // When set to true, the task will be executed as soon as possible, regardless of its scheduled nextPollTime.
    // After the task is executed, this flag will be reset to false.
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

    bool addModbusRequest(MBTask &t);
    void handleTimeouts();
    void startPendingTasks();

    void onData(ModbusMessage response, uint32_t token);
    void onError(Modbus::Error error, uint32_t token);

    MBTask *findTaskByToken(uint32_t token);
};
