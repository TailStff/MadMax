#ifndef MMVARIABLE_H
#define MMVARIABLE_H

#include "IVariableValue.h"
#include "IPrimitive.h"
#include <HardwareSerial.h>
#include "ExecutionEnv.h"

namespace MadMax
{
    template <class T>
    struct __attribute__((packed)) mmVariablePersistencyValues
    {
        T value;
    };

    template <class T>
    class mmVariable : public IVariableValue, public IPrimitiveTyped<mmVariablePersistencyValues<T>>
    {

    private:
        T value;

        ExecutionEnv *executionEnv;

    public:
        // Constructors
        mmVariable(ExecutionEnv *_executionEnv, mmVariablePersistencyValues<T> data = {.value = static_cast<T>(0)});
        ~mmVariable() = default;

        /// @brief Function that SET new value to the variable
        /// @param value The new value to SET
        /// @return Return true if setted value is different from the previous, false if there is no changes
        bool SetValue(T value);

        /// @brief Function that return the current value of the variable
        /// @return The current value of the variable
        T GetValue() const { return value; }

        /// @brief Static function that give the size of the serialized data of the object, here we just serialize the value
        /// @return Size in bytes of the serialized data of the object
        static size_t GetSerializedSize() { return sizeof(T); }

        /// @brief Function that fill the given persistencyValues struct with the current values of the object that we want to serialize
        /// @param persistencyValues Output struct that will receive the current values of the object
        /// @remarks This function override the GetPersistencyValues function of the ISerialize interface, it will be called by the provider when we want to save persistency values of the object
        void GetPersistencyValues(mmVariablePersistencyValues<T> &persistencyValues) const; // override;

        /// @brief Function that fill the given byte vector with the serialized persistency values of the object
        /// @param data Output vector that will receive the serialized bytes
        void GetBytesFromData(std::vector<uint8_t> &data) const override;

        void SetDataFromBytes(std::vector<uint8_t> &data) override;

#pragma region IVariableValue
        /// @brief Function that return the current value of the variable as a mmVariableValue variant, this will be used for generic access to variable value without knowing its type
        /// @return The current value of the variable as a mmVariableValue variant
        mmVariableValue GetVariantValue() const override;

        /// @brief Function that SET the variable value from a mmVariableValue variant, this will be used for generic access to variable value without knowing its type
        /// @param v The new value to SET as a mmVariableValue variant
        /// @return Return true if setted value is different from the previous, false if there is no changes or if the type of the variant value is not compatible with the variable type
        bool SetVariantValue(const mmVariableValue &v) override;
#pragma endregion IVariableValue

    };
}

#include "mmVariable.tpp"

#endif