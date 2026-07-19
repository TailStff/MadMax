#include "IVariableValue.h"
#include "Variable.h"

namespace MadMax
{
    class VariableModbusMapper
    {
    private:
        ExecutionEnv *executionEnv;

    public:
        VariableModbusMapper(ExecutionEnv *executionEnv)
        {
            this->executionEnv = executionEnv;
        }

        bool ExposeToModbus(const IPrimitive &obj, uint32_t address) const
        {
            const auto *primitive = obj.AsVariableValue();

            if (!primitive)
                return false;

            const VariableValue &varValue = primitive->GetVariantValue();

            // Copy the last saved value to modbus memory space, if it's valid, we consider that an address of -1 is an invalid address that mean that the variable is not associated to any Modbus register, this allow to create variable that are not exposed through Modbus if we want to
            if (address != -1)
            {
                // We consider that the address is a 32 bits integer where the 16 most significant bits represent the Modbus memory space (for example, holding registers, input registers, coils, discrete inputs) and the 16 least significant bits represent the Modbus address in that memory space, this allow to associate variables to different types of Modbus registers and not only holding registers
                uint8_t modbusMemorySpace = (address & 0x00FF0000) >> 16;

                // Holding registers
                if (modbusMemorySpace == 4)
                {
                    // We associate the variable to the Modbus register using the modbus server manager
                    uint16_t modbusAddress = address & 0x0000FFFF;

                    std::visit(
                        [&](const auto &value)
                        {
                            using T = std::decay_t<decltype(value)>;
                            auto *ptr = executionEnv->GetModbusServerMemoryManager()->AssociateHoldingRegister<T>(modbusAddress);

                            if (ptr)
                                *ptr = value;
                        },
                        varValue);
                }
            }

            return false;
        }

        bool GetFromModbus(IPrimitive &obj, uint32_t address) const
        {
            auto *primitive = obj.AsVariableValue();

            if (!primitive)
                return false;

            const VariableValue &varValue = primitive->GetVariantValue();

            // Copy the last saved value to modbus memory space, if it's valid, we consider that an address of -1 is an invalid address that mean that the variable is not associated to any Modbus register, this allow to create variable that are not exposed through Modbus if we want to
            if (address != -1)
            {
                // We consider that the address is a 32 bits integer where the 16 most significant bits represent the Modbus memory space (for example, holding registers, input registers, coils, discrete inputs) and the 16 least significant bits represent the Modbus address in that memory space, this allow to associate variables to different types of Modbus registers and not only holding registers
                uint8_t modbusMemorySpace = (address & 0x00FF0000) >> 16;

                // Holding registers
                if (modbusMemorySpace == 4)
                {
                    // We associate the variable to the Modbus register using the modbus server manager, we also store the Modbus address in the provider to be able to retrieve it later if needed
                    uint16_t modbusAddress = address & 0x0000FFFF;

                    std::visit(
                        [&](const auto &value)
                        {
                            using T = std::decay_t<decltype(value)>;
                            auto *ptr = executionEnv->GetModbusServerMemoryManager()->AssociateHoldingRegister<T>(modbusAddress);

                            if (ptr)
                                primitive->SetVariantValue(VariableValue(*ptr));
                        },
                        varValue);

                    return true;
                }
            }

            return false;
        }
    };
}