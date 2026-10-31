#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <vector>

enum class WordOrder
{
    HighWordFirst, // registre de poids fort en premier
    LowWordFirst   // registre de poids faible en premier
};

template <typename T>
bool ModbusRegistersFrameDecoder(const std::vector<uint16_t> &regs, size_t offset, T &out, WordOrder order = WordOrder::HighWordFirst)
{
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
    static_assert(sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "T must occupy 1, 2 or 4 Modbus registers");

    const size_t wordCount = sizeof(T) / 2;

    if (offset > regs.size() || wordCount > regs.size() - offset)
        return false; // offset outside limits or not enough registers to decode

    uint64_t bits = 0;

    // Combine the Modbus registers into a single integer representation (64 bits maximum) based on the specified word order.
    for (size_t i = 0; i < wordCount; ++i)
    {
        const size_t index = (order == WordOrder::HighWordFirst) ? i : wordCount - 1 - i;
        bits = (bits << 16) | regs[offset + index];
    }

    // ESP32 : représentation native little-endian.
    std::memcpy(&out, &bits, sizeof(T));

    return true;
}

bool ModbusCoilsFrameDecoder(const std::vector<uint16_t> &regs, uint16_t offset, bool &out)
{
    uint16_t wordIndex = offset / 16;
    uint16_t bitIndex = offset % 16;

    if (wordIndex >= regs.size())
        return false; // offset outside limits

    out = (regs[wordIndex] & (1 << bitIndex)) != 0;
    
    return true;
}
