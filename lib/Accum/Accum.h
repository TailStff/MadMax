#ifndef MADMAXCOUNT_H
#define MADMAXCOUNT_H

#include "IVariableValue.h"
#include "IPrimitive.h"
#include <HardwareSerial.h>
#include "AccumStatus.h"
#include "ExecutionEnv.h"

namespace MadMax
{
  template <class T>
  struct __attribute__((packed)) AccumPersistencyValues
  {
    T value;
  };

  template <class T>
  class Accum : public IVariableValue, public ISerializable/*, public IPrimitiveTyped<AccumPersistencyValues<T>>*/
  {

  private:
    bool memInput;
    bool memResetTrigger;

    ExecutionEnv *executionEnv;

    AccumStatus<T> status;

  public:
    // Constructors
    Accum(ExecutionEnv *_executionEnv, AccumPersistencyValues<T> data = {.value = static_cast<T>(0)});
    ~Accum() = default;

    /// @brief Function that SET new value to the variable
    /// @param value The new value to SET
    /// @return Return true if setted value is different from the previous, false if there is no changes
    bool SetValue(T value);

    /// @brief Function that return the current value of the variable
    /// @return The current value of the variable
    T GetValue() const { return this->status.value; }

    /// @brief Static function that give the size of the serialized data of the object, here we just serialize the value
    /// @return Size in bytes of the serialized data of the object
    static size_t GetSerializedSize() { return sizeof(T); }

    T Evaluate(bool input, T increment, bool resetTrigger = false, T resetValue = static_cast<T>(0));

    void Reset(T resetValue = static_cast<T>(0));

    void GetPersistencyValues(AccumPersistencyValues<T> &persistencyValues) const; // override;
    void GetBytesFromData(std::vector<uint8_t> &data) const override;
    void SetDataFromBytes(std::vector<uint8_t> &data) override;

#pragma region IVariableValue
    /// @brief Function that return the current value of the variable as a mmVariableValue variant, this will be used for generic access to variable value without knowing its type
    /// @return The current value of the variable as a mmVariableValue variant
    VariableValue GetVariantValue() const override;

    /// @brief Function that SET the variable value from a mmVariableValue variant, this will be used for generic access to variable value without knowing its type
    /// @param v The new value to SET as a mmVariableValue variant
    /// @return Return true if setted value is different from the previous, false if there is no changes or if the type of the variant value is not compatible with the variable type
    bool SetVariantValue(const VariableValue &v) override;

    // TBD
    void WriteToModbus(int32_t address) override;

#pragma endregion IVariableValue

    const ISerializable *AsSerializable() const override { return this; }
    ISerializable *AsSerializable() override { return this; }

    const IVariableValue *AsVariableValue() const override { return this; }
    IVariableValue *AsVariableValue() override { return this; }
  };
}

#include "Accum.tpp"

#endif