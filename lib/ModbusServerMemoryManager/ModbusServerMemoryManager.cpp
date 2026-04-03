#include "ModbusServerMemoryManager.h"

namespace MadMax
{
    ModbusServerMemoryManager::ModbusServerMemoryManager()
    {
        coils = new CoilData(sizeOfCoils);
        discreteInputs = new CoilData(sizeOfDiscreteInputs);

        holdingRegisters = (uint16_t *)heap_caps_aligned_alloc(8, sizeOfHoldingRegisters * sizeof(uint16_t), MALLOC_CAP_8BIT); // on ESP32, normal malloc does not guarantee 8-bit alignment, which is required for uint16_t access. So we use heap_caps_aligned_alloc with MALLOC_CAP_8BIT to ensure this.
        inputRegisters = (uint16_t *)heap_caps_aligned_alloc(8, sizeOfInputRegisters * sizeof(uint16_t), MALLOC_CAP_8BIT);     // on ESP32, normal malloc does not guarantee 8-bit alignment, which is required for uint16_t access. So we use heap_caps_aligned_alloc with MALLOC_CAP_8BIT to ensure this.

        memset(inputRegisters, 0, sizeOfInputRegisters * sizeof(uint16_t));     // Initilize input registers to 0
        memset(holdingRegisters, 0, sizeOfHoldingRegisters * sizeof(uint16_t)); // Initilize holding registers to 0
    }

    ModbusServerMemoryManager::~ModbusServerMemoryManager()
    {
        delete[] coils;
        delete[] discreteInputs;
        heap_caps_free(holdingRegisters);
        heap_caps_free(inputRegisters);
    }

    CoilData *ModbusServerMemoryManager::GetCoilsPtr()
    {
        return coils;
    }

    CoilData *ModbusServerMemoryManager::GetDiscreteInputsPtr()
    {
        return discreteInputs;
    }

    uint16_t *ModbusServerMemoryManager::GetInputRegistersPtr()
    {
        return inputRegisters;
    }

    uint16_t *ModbusServerMemoryManager::GetHoldingRegistersPtr()
    {
        return holdingRegisters;
    }

    void ModbusServerMemoryManager::addHoldingRegisters(ModbusMessage *response, uint16_t addr, uint16_t words)
    {
        if (CheckHoldingRegisterOverFlow(addr, words))
            return;

        for (uint8_t i = 0; i < words; ++i)
        {
            response->add(holdingRegisters[addr + i]);
        }
    }

    void ModbusServerMemoryManager::addInputRegisters(ModbusMessage *response, uint16_t addr, uint16_t words)
    {
        if (CheckInputRegisterOverFlow(addr, words))
            return;

        for (uint8_t i = 0; i < words; ++i)
        {
            response->add(inputRegisters[addr + i]);
        }
    }

    bool ModbusServerMemoryManager::CheckCoilOverFlow(uint16_t address, uint16_t quantity)
    {
        return (address + quantity) > sizeOfCoils;
    }

    bool ModbusServerMemoryManager::CheckDiscreteInputOverFlow(uint16_t address, uint16_t quantity)
    {
        return (address + quantity) > sizeOfDiscreteInputs;
    }

    bool ModbusServerMemoryManager::CheckHoldingRegisterOverFlow(uint16_t address, uint16_t quantity)
    {
        return (address + quantity) > sizeOfHoldingRegisters;
    }

    bool ModbusServerMemoryManager::CheckInputRegisterOverFlow(uint16_t address, uint16_t quantity)
    {
        return (address + quantity) > sizeOfInputRegisters;
    }
}