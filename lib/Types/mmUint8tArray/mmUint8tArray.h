#pragma once
#include <vector>
#include <cstdint>
#include <ArduinoJson.h>

struct mmUint8tArray
{
    std::vector<uint8_t> data;

    mmUint8tArray() = default;
    mmUint8tArray(const std::vector<uint8_t> &d) : data(d) {}
    mmUint8tArray(std::vector<uint8_t> &&d) : data(std::move(d)) {}

    size_t size() const { return data.size(); }
    uint8_t &operator[](size_t i) { return data[i]; }
    const uint8_t &operator[](size_t i) const { return data[i]; }
    void push_back(uint8_t b) { data.push_back(b); }
};

// Spécialisation ArduinoJson pour mmUint8tArray
namespace ArduinoJson
{
    template <>
    struct Converter<mmUint8tArray>
    {
        static void toJson(const mmUint8tArray &src, JsonVariant dst)
        {
            JsonArray arr = dst.to<JsonArray>();
            for (uint8_t b : src.data)
            {
                arr.add(b);
            }
        }
    };
}