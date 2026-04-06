#pragma once

#include <vector>
#include <stdio.h>

namespace MadMax
{
    class ISerializable
    {
    public:
        virtual ~ISerializable() = default;

        /// @brief Virtual function to implement Persistency data in bytes array format
        /// @param data Reference to the vector that will receive the bytes that represent the object persistency values
        virtual void GetBytesFromData(std::vector<uint8_t> &data) const = 0;

        /// @brief Virtual function to implement Persistency data from bytes array format
        /// @param data Reference to the vector that contains the bytes that represent the object persistency values
        virtual void SetDataFromBytes(std::vector<uint8_t> &data) = 0;
    };
}