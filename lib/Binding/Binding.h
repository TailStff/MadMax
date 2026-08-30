#pragma once

#include <memory>

#include "IPrimitive.h"
#include "ProtocolBinding.h"

namespace MadMax
{
    class Binding
    {
    private:
        IPrimitive *object;
        std::unique_ptr<ProtocolBinding> protocolBinding;

    public:
        Binding(IPrimitive &object, std::unique_ptr<ProtocolBinding> protocolBinding) : object(&object), protocolBinding(std::move(protocolBinding))
        {
        }

        IPrimitive *GetObject() const
        {
            return object;
        }

        ProtocolBinding *GetProtocolBinding() const
        {
            return protocolBinding.get();
        }
    };
}