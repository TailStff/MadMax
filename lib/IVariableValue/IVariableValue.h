#pragma once

#include "IPrimitive.h"
#include "VariableValue.h"

namespace MadMax
{
    class IVariableValue : public IPrimitive
    {
    public:
        virtual ~IVariableValue() = default;

        virtual VariableValue GetVariantValue() const = 0;
        virtual bool SetVariantValue(const VariableValue &value) = 0;

        // TBD
        virtual bool ReadFromModbus(int32_t address) = 0;
        virtual bool WriteToModbus(int32_t address) = 0;
    };
}