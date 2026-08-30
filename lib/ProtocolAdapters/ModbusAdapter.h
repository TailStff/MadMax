#pragma once

#include <vector>
#include <cstdint>

#include "IVariableValue.h"
#include "VariableValue.h"
#include "DataType.h"

#include "IProtocolAdapter.h"
#include "Binding.h"
#include "ModbusBinding.h"
#include "ModbusServerManager.h"
#include "ModbusServerMemoryManager.h"
#include "ModbusServerWriteEvent.h"



namespace MadMax
{
    class ModbusAdapter : public IProtocolAdapter
    {
    private:
        ModbusServerManager &serverManager;
        ModbusServerMemoryManager &memoryManager;

        // Non owning.
        // Les Binding sont possédés par BindingManager.
        std::vector<Binding *> bindings;

    public:
        ModbusAdapter(ModbusServerManager &serverManager, ModbusServerMemoryManager &memoryManager);
        ~ModbusAdapter() = default;

        ProtocolType GetProtocolType() const override { return ProtocolType::Modbus; }

        bool Bind(Binding &binding) override;
        void OnWrite(const ModbusWriteEvent &event);
        void PropertyChanged(Binding &binding) override;

    private:
        bool IsSupported(const Binding &binding) const;
        bool GetModbusAddress(const Binding &binding, uint32_t &address) const;
        size_t GetWordSize(const IPrimitive &object) const;
        bool CheckCollision(uint16_t bindingAddress, uint16_t bindingLength, uint16_t writeAddress, uint16_t writeLength) const;
        bool ExposeToModbus(Binding &binding);
        bool GetFromModbus(Binding &binding);
    };
}