#pragma once
#include <variant>
#include "DataType.h"
#include <../Types/mmUint8tArray/mmUint8tArray.h>
#include <../Types/mmBoolArray/mmBoolArray.h>

namespace MadMax
{
    using VariableValue = std::variant<
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

    inline DataType GetDataType(const VariableValue &v)
    {
        return std::visit(
            [](auto &&val) -> DataType
            {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, float>)
                    return DataType::floatType;
                if constexpr (std::is_same_v<T, double>)
                    return DataType::doubleType;
                if constexpr (std::is_same_v<T, int64_t>)
                    return DataType::int64Type;
                if constexpr (std::is_same_v<T, uint64_t>)
                    return DataType::uint64Type;
                if constexpr (std::is_same_v<T, int32_t>)
                    return DataType::int32Type;
                if constexpr (std::is_same_v<T, uint32_t>)
                    return DataType::uint32Type;
                if constexpr (std::is_same_v<T, int16_t>)
                    return DataType::int16Type;
                if constexpr (std::is_same_v<T, uint16_t>)
                    return DataType::uint16Type;
                if constexpr (std::is_same_v<T, int8_t>)
                    return DataType::int8Type;
                if constexpr (std::is_same_v<T, uint8_t>)
                    return DataType::uint8Type;
                if constexpr (std::is_same_v<T, bool>)
                    return DataType::boolType;
                if constexpr (std::is_same_v<T, mmUint8tArray>)
                    return DataType::arrayOfUint8Type;
                if constexpr (std::is_same_v<T, mmBoolArray>)
                    return DataType::arrayOfBoolType;

                return DataType::unknownType;
            },
            v);
    }
}