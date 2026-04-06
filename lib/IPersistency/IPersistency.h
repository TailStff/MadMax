#pragma once

#include <vector>

#include "ISerializable.h"

namespace MadMax
{
    // IPersistency.h — interface pour les primitives typées
    template <typename T>
    class IPersistency : public ISerializable
    {
    public:
        virtual void GetPersistencyValues(T &persistencyValues) const = 0;
    };
}