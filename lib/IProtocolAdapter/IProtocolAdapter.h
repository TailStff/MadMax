#pragma once

#include "ProtocolType.h"
#include "IPrimitive.h"
#include "ProtocolBinding.h"

namespace MadMax
{
    /// @brief ahead definition, interface for protocol adapters that allow to bind IPrimitive objects to specific protocol bindings
    class Binding;

    class IProtocolAdapter
    {
    public:
        virtual ~IProtocolAdapter() = default;
        
        /// @brief Gets the protocol type supported by this adapter by returning the corresponding ProtocolType enum value (polymorphic behavior)
        /// @return The ProtocolType enum value corresponding to the protocol type supported by this adapter
        virtual ProtocolType GetProtocolType() const = 0;

        virtual bool Bind(Binding &binding) = 0;
        virtual void PropertyChanged(Binding &binding) = 0;
    };
}