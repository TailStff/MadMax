#pragma once

#include <stdio.h>

class ModbusRegisters
{
public:
    ModbusRegisters(uint16_t _count = 0)
    {
        count = _count;
        rawdata = new uint16_t[count];
        memset(rawdata, 0, count << 1);
    }

    ~ModbusRegisters()
    {
        delete[] rawdata;
    }

    template <typename T>
    bool set(uint16_t index, const T &value)
    {
        static_assert(std::is_trivially_copyable<T>::value, "T must be a trivially copyable type");

        const size_t byteCount = sizeof(T);
        const size_t registersNeeded = (byteCount + 1) >> 1;

        if (index > count || registersNeeded > static_cast<size_t>(count - index))
            return false;

        auto *destination = reinterpret_cast<uint8_t *>(rawdata + index);

        memset(destination, 0, registersNeeded << 1);
        memcpy(destination, &value, byteCount);

        return true;
    }

    uint16_t *getRawData() { return rawdata; }
    const uint16_t *getRawData() const { return rawdata; }

    /// @brief Get number of registers
    /// @return
    uint16_t getSize() const { return count; }

private:
    uint16_t *rawdata;
    uint16_t count; // number of registers (16 bits each)
};