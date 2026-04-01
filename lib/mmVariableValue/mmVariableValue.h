#pragma once
#include <variant>
#include "mmDataType.h"
#include <../Types/mmUint8tArray/mmUint8tArray.h>
#include <../Types/mmBoolArray/mmBoolArray.h>

namespace MadMax
{
    using mmVariableValue = std::variant<
        float,
        double,
        int64_t,
        int32_t,
        int16_t,
        int8_t,
        uint64_t,
        uint32_t,
        uint16_t,
        uint8_t,
        bool,
        mmUint8tArray,
        mmBoolArray>;

    inline mmDataType GetDataType(const mmVariableValue &v)
    {
        return std::visit(
            [](auto &&val) -> mmDataType
            {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, float>)
                    return mmDataType::floatType;
                if constexpr (std::is_same_v<T, double>)
                    return mmDataType::doubleType;
                if constexpr (std::is_same_v<T, int64_t>)
                    return mmDataType::int64Type;
                if constexpr (std::is_same_v<T, uint64_t>)
                    return mmDataType::uint64Type;
                if constexpr (std::is_same_v<T, int32_t>)
                    return mmDataType::int32Type;
                if constexpr (std::is_same_v<T, uint32_t>)
                    return mmDataType::uint32Type;
                if constexpr (std::is_same_v<T, int16_t>)
                    return mmDataType::int16Type;
                if constexpr (std::is_same_v<T, uint16_t>)
                    return mmDataType::uint16Type;
                if constexpr (std::is_same_v<T, int8_t>)
                    return mmDataType::int8Type;
                if constexpr (std::is_same_v<T, uint8_t>)
                    return mmDataType::uint8Type;
                if constexpr (std::is_same_v<T, bool>)
                    return mmDataType::boolType;
                if constexpr (std::is_same_v<T, mmUint8tArray>)
                    return mmDataType::arrayOfUint8Type;
                if constexpr (std::is_same_v<T, mmBoolArray>)
                    return mmDataType::arrayOfBoolType;

                return mmDataType::unknownType;
            },
            v);
    }
}