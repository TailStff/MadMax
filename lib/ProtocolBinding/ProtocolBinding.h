#pragma once

#include "ProtocolType.h"

namespace MadMax
{
    /// @brief ahead definition
    class ModbusBinding;
    class BacnetBinding;

    class ProtocolBinding
    {
    public:
        virtual ~ProtocolBinding() = default;
        virtual ProtocolType GetProtocolType() const = 0;
        virtual const std::string &GetProperty() const = 0;

        virtual const ModbusBinding *AsModbusBinding() const
        {
            return nullptr;
        }

        virtual ModbusBinding *AsModbusBinding()
        {
            return nullptr;
        }

    };
}