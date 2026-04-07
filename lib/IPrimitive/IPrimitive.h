#pragma once

namespace MadMax
{
    class ISerializable;    // Forward declaration
    class IVariableValue;   // Forward declaration
    class DigitalEquipment; // Forward declaration
    class PumpSwap;         // Forward declaration
    class RunTime;          // Forward declaration
    class TPulse;           // Forward declaration

    class IPrimitive
    {
    public:
        virtual ~IPrimitive() = default;

        const virtual ISerializable *AsSerializable() const { return nullptr; }
        const virtual IVariableValue *AsVariableValue() const { return nullptr; }
        const virtual DigitalEquipment *AsDigitalEquipment() const { return nullptr; }
        const virtual RunTime *AsRunTime() const { return nullptr; }
        const virtual PumpSwap *AsPumpSwap() const { return nullptr; }
        const virtual TPulse *AsTPulse() const { return nullptr; }

        virtual ISerializable *AsSerializable() { return nullptr; }
        virtual IVariableValue *AsVariableValue() { return nullptr; }
        virtual DigitalEquipment *AsDigitalEquipment() { return nullptr; }
        virtual RunTime *AsRunTime() { return nullptr; }
        virtual PumpSwap *AsPumpSwap() { return nullptr; }
        virtual TPulse *AsTPulse() { return nullptr; }
    };
}