#pragma once

/// new

#include <string>

#include "VariableValue.h"

namespace MadMax
{
    class VariableBase
    {
    public:
        virtual const std::string &GetName() const = 0;
        virtual void SetFromVariant(const VariableValue &) = 0;
        virtual VariableValue ToVariant() const = 0;
    };
}