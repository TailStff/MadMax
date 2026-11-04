#pragma once

class ModbusSingleBit
{
public:
    ModbusSingleBit(bool value = false) : rawdata(value ? 0xFF00 : 0x0000) {}
    ~ModbusSingleBit() = default;

    void set(bool value) { rawdata = value ? 0xFF00 : 0x0000; }
    bool get() const { return rawdata == 0xFF00; }
    uint16_t *getRawData() { return &rawdata; }

private:
    uint16_t rawdata = 0x0000;
};