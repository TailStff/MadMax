#ifndef MMCOUNT_H
#define MMCOUNT_H

#include "IVariableValue.h"
#include "IPrimitive.h"
#include <HardwareSerial.h>
#include "ExecutionEnv.h"
#include "structDelayStatus.h"

template <class T>
struct __attribute__((packed)) mmAccumPersistencyValues
{
  T value;
};

template <class T>
class mmAccum : public IVariableValue, public IPrimitiveTyped<mmAccumPersistencyValues<T>>
{

private:
  bool memInput, memResetTrigger;
  T value;

  ExecutionEnv *executionEnv;

public:
  // Constructors
  mmAccum(ExecutionEnv *_executionEnv, mmAccumPersistencyValues<T> data = {.value = static_cast<T>(0)});
  ~mmAccum() = default;

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

  T Evaluate(bool input, T increment, bool resetTrigger = false, T resetValue = static_cast<T>(0));

  void Reset(T resetValue = static_cast<T>(0));

  void GetPersistencyValues(mmAccumPersistencyValues<T> &persistencyValues) const; // override;
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

/*#pragma region IObjectDTO
  bool GetDTO(DTOBase &dto) const override;
  bool GetDetailDTO(DTOBase &dto) const override;
#pragma endregion IObjectDTO*/
};

#include "mmAccum.tpp"

#endif