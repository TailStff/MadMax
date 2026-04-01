#pragma once

#include <vector>
#include <stdio.h>


class ISerializableBase
{
public:
    virtual ~ISerializableBase() = default;

    /// @brief Virtual function to implement Persistency data in bytes array format
    /// @param data 
    virtual void GetBytesFromData(std::vector<uint8_t> &data) const = 0;

    virtual void SetDataFromBytes(std::vector<uint8_t> &data) = 0;
};