#pragma once

#include <functional>
#include "ModbusFunction.h"
#include "ModbusTypeDefs.h"
#include "ModbusTaskEnum.h"
#include "ModbusSingleBit.h"
#include "ModbusRegisters.h"

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

    /// @brief Prepares a task to write a single coil
    /// @param slave Slave address of the Modbus device to communicate with.
    /// @param address Starting address of the Modbus operation.
    /// @param pollIntervalMs Polling interval in milliseconds.
    /// @param obj Object containing the coil value to write.
    /// @param callback Callback function to be invoked upon task completion or error.
    void PrepareWriteSingleCoilTask(uint8_t slave, uint16_t address, uint32_t pollIntervalMs, ModbusSingleBit *obj, std::function<void(const MBTask &)> callback);

    /// @brief Prepares a task to write a single register
    /// @param slave Slave address of the Modbus device to communicate with.
    /// @param address Starting address of the Modbus operation.
    /// @param pollIntervalMs Polling interval in milliseconds.
    /// @param obj Object containing the register value to write.
    /// @param callback Callback function to be invoked upon task completion or error.
    void PrepareWriteSingleRegisterTask(uint8_t slave, uint16_t address, uint32_t pollIntervalMs, uint16_t *obj, std::function<void(const MBTask &)> callback);

    /// @brief Prepares a task to write multiple registers
    /// @param slave Slave address of the Modbus device to communicate with.
    /// @param address Starting address of the Modbus operation.
    /// @param pollIntervalMs Polling interval in milliseconds.
    /// @param obj Object containing the register values to write.
    /// @param callback Callback function to be invoked upon task completion or error.
    void PrepareWriteMultipleRegistersTask(uint8_t slave, uint16_t address, uint32_t pollIntervalMs, ModbusRegisters *obj, std::function<void(const MBTask &)> callback);

    /// @brief Prepares a task to read input registers
    /// @param slave Slave address of the Modbus device to communicate with.
    /// @param address Starting address of the Modbus operation.
    /// @param quantity Quantity of input registers to read.
    /// @param pollIntervalMs Polling interval in milliseconds.
    /// @param callback Callback function to be invoked upon task completion or error.
    void PrepareReadInputRegistersTask(uint8_t slave, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, std::function<void(const MBTask &)> callback);

    /// @brief Prepares a task to read holding registers
    /// @param slave Slave address of the Modbus device to communicate with.
    /// @param address Starting address of the Modbus operation.
    /// @param quantity Quantity of holding registers to read.
    /// @param pollIntervalMs Polling interval in milliseconds.
    /// @param callback Callback function to be invoked upon task completion or error.
    void PrepareReadHoldingRegistersTask(uint8_t slave, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, std::function<void(const MBTask &)> callback);

    /// @brief Prepares a task to read coils
    /// @param slave Slave address of the Modbus device to communicate with.
    /// @param address Starting address of the Modbus operation.
    /// @param quantity Quantity of coils to read.
    /// @param pollIntervalMs Polling interval in milliseconds.
    /// @param callback Callback function to be invoked upon task completion or error.
    void PrepareReadCoilsTask(uint8_t slave, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, std::function<void(const MBTask &)> callback);

    /// @brief Prepares a task to read discrete inputs
    /// @param slave Slave address of the Modbus device to communicate with.
    /// @param address Starting address of the Modbus operation.
    /// @param quantity Quantity of discrete inputs to read.
    /// @param pollIntervalMs Polling interval in milliseconds.
    /// @param callback Callback function to be invoked upon task completion or error.
    void PrepareReadDiscreteInputsTask(uint8_t slave, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, std::function<void(const MBTask &)> callback);

    /// @brief Prepares a task to write multiple coils
    /// @param slave Slave address of the Modbus device to communicate with.
    /// @param address Starting address of the Modbus operation.
    /// @param quantity Quantity of coils to write.
    /// @param pollIntervalMs Polling interval in milliseconds.
    /// @param writeValuePtr Pointer to the array of coil values to write.
    /// @param callback Callback function to be invoked upon task completion or error.
    void PrepareWriteMultipleCoilsTask(uint8_t slave, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, uint8_t *writeValuePtr, std::function<void(const MBTask &)> callback);

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
    uint32_t timeoutMs = 3000;

    uint8_t retryCount = 0;
    uint8_t retryMax = 3;

    uint32_t token;

    Modbus::Error lastError = Modbus::SUCCESS;

    // callback
    std::function<void(const MBTask &)> callback;

protected:
private:
};