/*#pragma once

/// new

#include "VariableBase.h"

namespace MadMax
{
    template <typename T>
    class VariableNew : public IVariable
    {
    public:
        VariableNew(const T &defaultValue = {}) : value(defaultValue)
        {
        }

        const T &Get() const
        {
            return value;
        }

        void Set(const T &v)
        {
            value = v;
        }

        VariableValue GetValue() const override
        {
            return value;
        }

        bool SetValue(const VariableValue &v) override
        {
            // 🔒 SAFE conversion uniquement
            if (const T *val = std::get_if<T>(&v))
            {
                value = *val;
                return true;
            }

            return false; // pas de cast implicite
        }

    private:
        T value;
    };
}*/