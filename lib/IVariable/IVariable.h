#pragma once

/// new

#include "VariableValue.h"
#include "IPrimitive.h"

namespace MadMax
{
    class IVariable : public ISerializableBase
    {
    public:
        virtual VariableValue GetValue() const = 0;
        virtual bool SetValue(const VariableValue &v) = 0;
    };
}