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
    };
}