#pragma once

#include <vector>
#include "ISerializableBase.h"

// IPersistency.h — interface pour les primitives typées
template <typename T>
class IPersistency : public ISerializableBase
{
public:
    virtual void GetPersistencyValues(T &persistencyValues) const = 0;
};
