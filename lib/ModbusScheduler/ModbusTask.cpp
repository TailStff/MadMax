#include "ModbusTask.h"
#include "ModbusSingleBit.h"
#include "ModbusRegisters.h"
#include <cstdint>

MBTask::MBTask() {}
MBTask::~MBTask() {}

/// @brief Prepares a task to write a single coil
/// @param slave Slave address of the Modbus device to communicate with.
/// @param address Starting address of the Modbus operation.
/// @param pollIntervalMs Polling interval in milliseconds.
/// @param obj Object containing the coil value to write.
/// @param callback Callback function to be invoked upon task completion or error.
void MBTask::PrepareWriteSingleCoilTask(uint8_t slave, uint16_t address, uint32_t pollIntervalMs, ModbusSingleBit *obj, std::function<void(const MBTask &)> callback)
{
    this->slave = slave;
    this->function = MBFunction::WriteSingleCoil;
    this->address = address;
    this->quantity = 1; // Single coil
    this->pollIntervalMs = pollIntervalMs;
    this->writeValuePtr = obj->getRawData();
    this->callback = callback;
}

/// @brief Prepares a task to write a single register
/// @param slave Slave address of the Modbus device to communicate with.
/// @param address Starting address of the Modbus operation.
/// @param pollIntervalMs Polling interval in milliseconds.
/// @param obj Object containing the register value to write.
/// @param callback Callback function to be invoked upon task completion or error.
void MBTask::PrepareWriteSingleRegisterTask(uint8_t slave, uint16_t address, uint32_t pollIntervalMs, uint16_t *obj, std::function<void(const MBTask &)> callback)
{
    this->slave = slave;
    this->function = MBFunction::WriteSingleRegister;
    this->address = address;
    this->quantity = 1; // Single register
    this->pollIntervalMs = pollIntervalMs;
    this->writeValuePtr = obj;
    this->callback = callback;
}

/// @brief Prepares a task to write multiple registers
/// @param slave Slave address of the Modbus device to communicate with.
/// @param address Starting address of the Modbus operation.
/// @param pollIntervalMs Polling interval in milliseconds.
/// @param obj Object containing the register values to write.
/// @param callback Callback function to be invoked upon task completion or error.
void MBTask::PrepareWriteMultipleRegistersTask(uint8_t slave, uint16_t address, uint32_t pollIntervalMs, ModbusRegisters *obj, std::function<void(const MBTask &)> callback)
{
    this->slave = slave;
    this->function = MBFunction::WriteMultipleRegisters;
    this->address = address;
    this->quantity = obj->getSize();
    this->pollIntervalMs = pollIntervalMs;
    this->writeValuePtr = obj->getRawData();
    this->callback = callback;
}

/// @brief Prepares a task to read input registers
/// @param slave Slave address of the Modbus device to communicate with.
/// @param address Starting address of the Modbus operation.
/// @param quantity Quantity of input registers to read.
/// @param pollIntervalMs Polling interval in milliseconds.
/// @param callback Callback function to be invoked upon task completion or error.
void MBTask::PrepareReadInputRegistersTask(uint8_t slave, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, std::function<void(const MBTask &)> callback)
{
    this->slave = slave;
    this->function = MBFunction::ReadInputRegisters;
    this->address = address;
    this->quantity = quantity;
    this->pollIntervalMs = pollIntervalMs;
    this->writeValuePtr = nullptr;
    this->callback = callback;
}

/// @brief Prepares a task to read holding registers
/// @param slave Slave address of the Modbus device to communicate with.
/// @param address Starting address of the Modbus operation.
/// @param quantity Quantity of holding registers to read.
/// @param pollIntervalMs Polling interval in milliseconds.
/// @param callback Callback function to be invoked upon task completion or error.
void MBTask::PrepareReadHoldingRegistersTask(uint8_t slave, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, std::function<void(const MBTask &)> callback)
{
    this->slave = slave;
    this->function = MBFunction::ReadHoldingRegisters;
    this->address = address;
    this->quantity = quantity;
    this->pollIntervalMs = pollIntervalMs;
    this->writeValuePtr = nullptr;
    this->callback = callback;
}

/// @brief Prepares a task to read coils
/// @param slave Slave address of the Modbus device to communicate with.
/// @param address Starting address of the Modbus operation.
/// @param quantity Quantity of coils to read.
/// @param pollIntervalMs Polling interval in milliseconds.
/// @param callback Callback function to be invoked upon task completion or error.
void MBTask::PrepareReadCoilsTask(uint8_t slave, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, std::function<void(const MBTask &)> callback)
{
    this->slave = slave;
    this->function = MBFunction::ReadCoils;
    this->address = address;
    this->quantity = quantity;
    this->pollIntervalMs = pollIntervalMs;
    this->writeValuePtr = nullptr;
    this->callback = callback;
}

/// @brief Prepares a task to read discrete inputs
/// @param slave Slave address of the Modbus device to communicate with.
/// @param address Starting address of the Modbus operation.
/// @param quantity Quantity of discrete inputs to read.
/// @param pollIntervalMs Polling interval in milliseconds.
/// @param callback Callback function to be invoked upon task completion or error.
void MBTask::PrepareReadDiscreteInputsTask(uint8_t slave, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, std::function<void(const MBTask &)> callback)
{
    this->slave = slave;
    this->function = MBFunction::ReadDiscreteInputs;
    this->address = address;
    this->quantity = quantity;
    this->pollIntervalMs = pollIntervalMs;
    this->writeValuePtr = nullptr;
    this->callback = callback;
}

/// @brief Prepares a task to write multiple coils
/// @param slave Slave address of the Modbus device to communicate with.
/// @param address Starting address of the Modbus operation.
/// @param quantity Quantity of coils to write.
/// @param pollIntervalMs Polling interval in milliseconds.
/// @param writeValuePtr Pointer to the array of coil values to write.
/// @param callback Callback function to be invoked upon task completion or error.
void MBTask::PrepareWriteMultipleCoilsTask(uint8_t slave, uint16_t address, uint16_t quantity, uint32_t pollIntervalMs, uint8_t *writeValuePtr, std::function<void(const MBTask &)> callback)
{
    this->slave = slave;
    this->function = MBFunction::WriteMultipleCoils;
    this->address = address;
    this->quantity = quantity;
    this->pollIntervalMs = pollIntervalMs;
    this->coilsValuePtr = writeValuePtr;
    this->callback = callback;
}