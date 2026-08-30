#include "BindingManager.h"

namespace MadMax
{
    bool BindingManager::RegisterProtocolAdapter(IProtocolAdapter &adapter)
    {
        for (auto *registeredAdapter : adapters)
        {
            if (registeredAdapter == &adapter)
                return false;
        }

        adapters.push_back(&adapter);
        return true;
    }

    bool BindingManager::Bind(IPrimitive &object, std::unique_ptr<ProtocolBinding> protocolBinding)
    {
        if (!protocolBinding)
            return false;

        auto binding = std::make_unique<Binding>(object, std::move(protocolBinding));

        ProtocolType protocol = binding->GetProtocolBinding()->GetProtocolType();

        for (auto *adapter : adapters)
        {
            if (adapter->GetProtocolType() == protocol)
            {
                if (!adapter->Bind(*binding))
                    return false;

                bindings.push_back(std::move(binding));
                return true;
            }
        }

        return false;
    }

    void BindingManager::PropertyChanged(IPrimitive &object, const std::string &property)
    {
        for (auto &binding : bindings)
        {
            if (binding->GetObject() != &object)
                continue;

            ProtocolBinding *protocolBinding = binding->GetProtocolBinding();

            if (!protocolBinding)
                continue;

            if (protocolBinding->GetProperty() != property)
                continue;

            ProtocolType protocol = protocolBinding->GetProtocolType();

            for (auto *adapter : adapters)
            {
                if (adapter->GetProtocolType() == protocol)
                {
                    adapter->PropertyChanged(*binding);
                }
            }
        }
    }
}