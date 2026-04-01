// IVariableValue.h
#pragma once
#include "mmVariableValue.h"

namespace MadMax
{
    class IVariableValue : public IPrimitive
    {
    public:
        virtual ~IVariableValue() = default;

        virtual mmVariableValue GetVariantValue() const = 0;
        virtual bool SetVariantValue(const mmVariableValue &value) = 0;
    };
}