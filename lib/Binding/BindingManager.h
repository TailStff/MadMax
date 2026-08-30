#pragma once

#include <memory>
#include <vector>

#include "Binding.h"
#include "IProtocolAdapter.h"

namespace MadMax
{
    class BindingManager
    {
    private:
        std::vector<std::unique_ptr<Binding>> bindings;
        std::vector<IProtocolAdapter *> adapters;

    public:
        bool RegisterProtocolAdapter(IProtocolAdapter &adapter);
        bool Bind(IPrimitive &object, std::unique_ptr<ProtocolBinding> protocolBinding);
        void PropertyChanged(IPrimitive &object, const std::string &property);
    };
}