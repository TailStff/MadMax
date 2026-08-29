#pragma once

#include "ProtocolBinding.h"
#include <cstdint>
#include <string>

namespace MadMax
{
    class BacnetBinding : public ProtocolBinding
    {
    private:
        std::string property;
        uint8_t objectType;
        uint32_t objectId;

    public:
        BacnetBinding(const std::string &property, uint8_t objectType, uint32_t objectId) : property(property), objectType(objectType), objectId(objectId)
        {
        }

        ProtocolType GetProtocolType() const override
        {
            return ProtocolType::Bacnet;
        }
    };
}