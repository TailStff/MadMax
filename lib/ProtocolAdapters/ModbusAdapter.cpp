#include "ModbusAdapter.h"

namespace MadMax
{
    ModbusAdapter::ModbusAdapter(ModbusServerManager &serverManager, ModbusServerMemoryManager &memoryManager) : serverManager(serverManager), memoryManager(memoryManager)
    {
        serverManager.RegisterWriteCallback(
            [this](const ModbusWriteEvent &event)
            {
                OnWrite(event);
            });
    }

    bool ModbusAdapter::IsSupported(const Binding &binding) const
    {
        auto *protocolBinding = binding.GetProtocolBinding();

        if (!protocolBinding)
            return false;

        if (protocolBinding->GetProtocolType() != ProtocolType::Modbus)
            return false;

        auto *modbusBinding = protocolBinding->AsModbusBinding();

        if (!modbusBinding)
            return false;

        // Pour l'instant, seule la propriété "value"
        // est supportée.
        if (modbusBinding->GetProperty() != "value")
            return false;

        return true;
    }

    bool ModbusAdapter::GetModbusAddress(const Binding &binding, uint32_t &address) const
    {
        auto *modbusBinding = binding.GetProtocolBinding()->AsModbusBinding();

        if (!modbusBinding)
            return false;

        address = modbusBinding->GetAddress();

        return true;
    }

    size_t ModbusAdapter::GetWordSize(const IPrimitive &object) const
    {
        auto *variable = object.AsVariableValue();

        if (!variable)
            return 0;

        VariableValue value = variable->GetVariantValue();
        DataType type = GetDataType(value);
        size_t byteSize = GetDataTypeSize(type);

        if (byteSize == 0)
            return 0;

        return (byteSize + 1) / 2;
    }

    bool ModbusAdapter::CheckCollision(uint16_t bindingAddress, uint16_t bindingLength, uint16_t writeAddress, uint16_t writeLength) const
    {
        uint16_t bindingEnd = bindingAddress + bindingLength;
        uint16_t writeEnd = writeAddress + writeLength;

        return (writeAddress < bindingEnd && writeEnd > bindingAddress);
    }

    bool ModbusAdapter::Bind(Binding &binding)
    {
        if (!IsSupported(binding))
            return false;

        IPrimitive *object = binding.GetObject();

        if (!object)
            return false;

        uint32_t address;

        if (!GetModbusAddress(binding, address))
            return false;

        if (address == UINT32_MAX)
            return false;

        uint8_t memorySpace = (address & 0x00FF0000) >> 16;

        // Première version :
        // uniquement Holding Registers.
        if (memorySpace != 4)
            return false;

        size_t words = GetWordSize(*object);

        if (words == 0)
            return false;

        uint16_t modbusAddress = address & 0x0000FFFF;

        if (memoryManager.CheckHoldingRegisterOverFlow(modbusAddress, words))
            return false;

        // Expose immédiatement la valeur actuelle.
        if (!ExposeToModbus(binding))
            return false;

        bindings.push_back(&binding);

        return true;
    }

    bool ModbusAdapter::ExposeToModbus(Binding &binding)
    {
        IPrimitive *object = binding.GetObject();

        if (!object)
            return false;

        auto *variable = object->AsVariableValue();

        if (!variable)
            return false;

        uint32_t address;

        if (!GetModbusAddress(binding, address))
            return false;

        uint8_t memorySpace = (address & 0x00FF0000) >> 16;

        if (memorySpace != 4)
            return false;

        uint16_t modbusAddress = address & 0x0000FFFF;

        const VariableValue &value = variable->GetVariantValue();

        bool success = false;

        std::visit(
            [&](const auto &value)
            {
                using T = std::decay_t<decltype(value)>;

                auto *ptr = memoryManager.AssociateHoldingRegister<T>(modbusAddress);

                if (ptr)
                {
                    *ptr = value;
                    success = true;
                }
            },
            value);

        return success;
    }

    void ModbusAdapter::OnWrite(const ModbusWriteEvent &event)
    {
        for (auto *binding : bindings)
        {
            if (!binding)
                continue;

            uint32_t address;

            if (!GetModbusAddress(*binding, address))
                continue;

            uint8_t memorySpace = (address & 0x00FF0000) >> 16;

            if (memorySpace != static_cast<uint8_t>(event.modbusSpace))
                continue;

            uint16_t bindingAddress = address & 0x0000FFFF;

            size_t bindingLength = GetWordSize(*binding->GetObject());

            if (bindingLength == 0)
                continue;

            if (!CheckCollision(bindingAddress, bindingLength, event.startAddr, event.count))
                continue;

            GetFromModbus(*binding);
        }
    }

    bool ModbusAdapter::GetFromModbus(Binding &binding)
    {
        IPrimitive *object = binding.GetObject();

        if (!object)
            return false;

        auto *variable = object->AsVariableValue();

        if (!variable)
            return false;

        uint32_t address;

        if (!GetModbusAddress(binding, address))
            return false;

        uint8_t memorySpace = (address & 0x00FF0000) >> 16;

        if (memorySpace != 4)
            return false;

        uint16_t modbusAddress = address & 0x0000FFFF;

        VariableValue current = variable->GetVariantValue();

        bool success = false;

        std::visit(
            [&](const auto &value)
            {
                using T =
                    std::decay_t<decltype(value)>;

                auto *ptr = memoryManager.AssociateHoldingRegister<T>(modbusAddress);

                if (ptr)
                {
                    variable->SetVariantValue(VariableValue(*ptr));
                    success = true;
                }
            },
            current);

        return success;
    }

    void ModbusAdapter::PropertyChanged(Binding &binding)
    {
        auto *protocolBinding = binding.GetProtocolBinding();

        if (!protocolBinding)
            return;

        // Pour l'instant, seul "value" est supporté.
        if (protocolBinding->GetProperty() != "value")
            return;

        ExposeToModbus(binding);
    }
}