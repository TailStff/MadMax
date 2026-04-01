#pragma once

#include <vector>

#include "ISerializableBase.h"

namespace MadMax
{
    // IPersistency.h — interface pour les primitives typées
    template <typename T>
    class IPersistency : public ISerializableBase
    {
    public:
        virtual void GetPersistencyValues(T &persistencyValues) const = 0;
    };
}