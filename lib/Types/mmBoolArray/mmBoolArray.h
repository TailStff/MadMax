#pragma once
#include <vector>
#include <cstdint>
#include <ArduinoJson.h>

struct mmBoolArray
{
    std::vector<bool> data;

    mmBoolArray() = default;
    mmBoolArray(const std::vector<bool> &d) : data(d) {}
    mmBoolArray(std::vector<bool> &&d) : data(std::move(d)) {}

    size_t size() const { return data.size(); }
    std::vector<bool>::reference operator[](size_t i) { return data[i]; }
    bool operator[](size_t i) const { return data[i]; } // retour par valeur pour const
};

// Spécialisation ArduinoJson pour mmBoolArray
namespace ArduinoJson
{
    template <>
    struct Converter<mmBoolArray>
    {
        static void toJson(const mmBoolArray &src, JsonVariant dst)
        {
            JsonArray arr = dst.to<JsonArray>();
            for (bool b : src.data)
            {
                arr.add(b);
            }
        }
    };
}