#pragma once

#include "ExecutionEnv.h"

namespace MadMax
{
    class IPersistable
    {
    public:
        virtual ~IPersistable() = default;

        // Méthode pure virtuelle : toute classe dérivée doit l’implémenter
        virtual void SavePersistencyValuesToStorage(const std::string &name) = 0;
        virtual void GetPersistencyValuesFromStorage(const std::string &name, uint8_t *data, size_t length) = 0;
    };
}