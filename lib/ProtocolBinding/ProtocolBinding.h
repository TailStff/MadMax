#pragma once

#include "ProtocolType.h"

namespace MadMax
{
    class ProtocolBinding
    {
    public:
        virtual ~ProtocolBinding() = default;
        virtual ProtocolType GetProtocolType() const = 0;
    };
}