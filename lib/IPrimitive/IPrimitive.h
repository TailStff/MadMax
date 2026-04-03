#pragma once

#include <vector>
#include <cstdio>

#include "ISerializableBase.h"
#include "IPersistency.h"
#include "IObjectDTO.h"

namespace MadMax
{
    class IPrimitive : public ISerializableBase
    {
    public:
        virtual ~IPrimitive() = default;
    };

    template <typename T>
    class IPrimitiveTyped
    {
    public:
        virtual ~IPrimitiveTyped() = default;
    };
}