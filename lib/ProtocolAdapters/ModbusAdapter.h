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

        /// @brief Binds a Binding to the ModbusAdapter, allowing the associated IPrimitive object to be exposed to the Modbus protocol.
        /// @param binding The Binding object to be bound to the ModbusAdapter.
        /// @return True if the binding was successful, false otherwise.
        bool Bind(Binding &binding) override;

        /// @brief Handles a Modbus write event from ModbusServer by checking for collisions with existing bindings and updating the corresponding IPrimitive objects if necessary.
        /// @param event The ModbusWriteEvent containing information about the write operation.
        void OnWrite(const ModbusWriteEvent &event);

        /// @brief Notifies the ModbusAdapter that a property of the associated IPrimitive object has changed, allowing the adapter to update the Modbus server accordingly. 
        /// @param binding The Binding object associated with the IPrimitive object whose property has changed.
        void PropertyChanged(Binding &binding) override;

    private:
        /// @brief Checks if a Binding is supported by the ModbusAdapter.
        /// @param binding The Binding object to check.
        /// @return True if the binding is supported, false otherwise.
        bool IsSupported(const Binding &binding) const;

        /// @brief Spectific to ModbusAdapter, gets the Modbus address from a ModbusBinding object data.
        /// @param binding The Binding object from which to retrieve the Modbus address.
        /// @param address A reference to a uint32_t variable where the Modbus address will be stored if successfully retrieved.
        /// @return True if the Modbus address was successfully retrieved, false otherwise.
        bool GetModbusAddress(const Binding &binding, uint32_t &address) const;

        size_t GetWordSize(const IPrimitive &object) const;
        bool CheckCollision(uint16_t bindingAddress, uint16_t bindingLength, uint16_t writeAddress, uint16_t writeLength) const;
        bool ExposeToModbus(Binding &binding);
        bool GetFromModbus(Binding &binding);
    };
}