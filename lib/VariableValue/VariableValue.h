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

    inline bool VariableValueFromJson(JsonVariantConst v, VariableValue &out)
    {
        // bool en premier (sinon true → 1)
        if (v.is<bool>())
        {
            out = v.as<bool>();
            return true;
        }

        // Entiers
        if (v.is<int64_t>())
        {
            out = v.as<int64_t>();
            return true;
        }

        if (v.is<int32_t>())
        {
            out = v.as<int32_t>();
            return true;
        }

        // Float / double
        if (v.is<float>())
        {
            out = v.as<float>();
            return true;
        }

        if (v.is<double>())
        {
            out = v.as<double>();
            return true;
        }

        // Array uint8 (ex: [1,2,3])
        if (v.is<JsonArray>())
        {
            JsonArrayConst arr = v.as<JsonArrayConst>();

            // Heuristique simple : tableau de bool ?
            bool isBoolArray = true;

            for (auto val : arr)
            {
                if (!val.is<bool>())
                {
                    isBoolArray = false;
                    break;
                }
            }

            if (isBoolArray)
            {
                mmBoolArray outArr;
                for (auto val : arr)
                    outArr.push_back(val.as<bool>());

                out = outArr;
                return true;
            }
            else
            {
                mmUint8tArray outArr;
                for (auto val : arr)
                    outArr.push_back(val.as<uint8_t>());

                out = outArr;
                return true;
            }
        }

        return false;
    }
}