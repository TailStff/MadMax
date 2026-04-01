#pragma once

#include "ExecutionEnv.h"

class IPersistable
{
public:
    virtual ~IPersistable() = default;

    // Méthode pure virtuelle : toute classe dérivée doit l’implémenter
    virtual void SavePersistencyValuesToMem(const std::string &name) = 0;
    virtual void GetPersistencyValuesFromMem(const std::string &name, uint8_t *data, size_t length) = 0;
};
