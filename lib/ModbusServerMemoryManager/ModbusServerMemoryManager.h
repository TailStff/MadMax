#ifndef MADMAXMODBUSSERVERMEMORYMANAGER_H
#define MADMAXMODBUSSERVERMEMORYMANAGER_H

#include <cstdio>
#include <vector>
#include <functional>
#include <stdint.h>

#include "CoilData.h"
#include "ModbusMessage.h"

namespace MadMax
{
    class ModbusServerMemoryManager
    {

    private:
        CoilData *coils;
        uint16_t sizeOfCoils = 1000;

        CoilData *discreteInputs;
        uint16_t sizeOfDiscreteInputs = 100;

        uint16_t *inputRegisters;
        uint16_t sizeOfInputRegisters = 100;

        uint16_t *holdingRegisters;
        uint16_t sizeOfHoldingRegisters = 1000;

    public:
        // Constructors
        ModbusServerMemoryManager();
        ~ModbusServerMemoryManager();

        CoilData *GetCoilsPtr();
        CoilData *GetDiscreteInputsPtr();
        uint16_t *GetInputRegistersPtr();
        uint16_t *GetHoldingRegistersPtr();

        void addHoldingRegisters(ModbusMessage *response, uint16_t addr, uint16_t words);
        void addInputRegisters(ModbusMessage *response, uint16_t addr, uint16_t words);

        bool CheckCoilOverFlow(uint16_t address, uint16_t quantity);
        bool CheckDiscreteInputOverFlow(uint16_t address, uint16_t quantity);
        bool CheckHoldingRegisterOverFlow(uint16_t address, uint16_t quantity);
        bool CheckInputRegisterOverFlow(uint16_t address, uint16_t quantity);

        template <typename T>
        T *AssociateHoldingRegister(uint16_t address)
        {
            // Ensure the address is properly aligned for type T and does not overflow
            uintptr_t byteAddr = address * 2;

            if (byteAddr % alignof(T) != 0)
                return nullptr;

            if (CheckHoldingRegisterOverFlow(address, sizeof(T) / 2))
                return nullptr;

            return reinterpret_cast<T *>(&holdingRegisters[address]);
        }

        template <typename T>
        T *AssociateInputRegister(uint16_t address)
        {
            // Ensure the address is properly aligned for type T and does not overflow
            uintptr_t byteAddr = address * 2;

            if (byteAddr % alignof(T) != 0)
                return nullptr;

            if (CheckInputRegisterOverFlow(address, sizeof(T) / 2))
                return nullptr;

            return reinterpret_cast<T *>(&inputRegisters[address]);
        }
    };
}

#endif