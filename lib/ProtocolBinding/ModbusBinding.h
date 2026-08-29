#pragma once

#include "ProtocolBinding.h"
#include <cstdint>
#include <string>

namespace MadMax
{
    class ModbusBinding : public ProtocolBinding
    {
    private:
        std::string property;
        uint32_t address;

    public:
        ModbusBinding(const std::string &property, uint32_t address) : property(property), address(address)
        {
        }

        ProtocolType GetProtocolType() const override
        {
            return ProtocolType::Modbus;
        }

        const uint32_t &GetAddress() const
        {
            return address;
        }
    };
}