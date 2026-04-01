#ifndef MMRUNTIME_H
#define MMRUNTIME_H

#include <stdio.h>
#include <HardwareSerial.h>
#include "IPrimitive.h"
#include "ExecutionEnv.h"

struct __attribute__((packed)) mmRunTimePersistencyValues
{
  uint64_t value;
};

class mmRunTime : public IPrimitiveTyped<mmRunTimePersistencyValues>, public IPrimitive
{

private:
  uint16_t cycle;
  uint64_t value;
  ExecutionEnv *executionEnv;

public:
  // Constructors
  mmRunTime(ExecutionEnv *_executionEnv, mmRunTimePersistencyValues data = {.value = 0});
  ~mmRunTime();

  uint64_t Evaluate(bool in);
  void Reset();
  void Reset(uint64_t value);
  uint64_t GetValue() { return value; }

  /// @brief Static function that give the size of the serialized data of the object, here we just serialize the uint64_t value
  /// @return Size in bytes of the serialized data of the object
  static size_t GetSerializedSize()
  {
    return sizeof(mmRunTimePersistencyValues);
  }

  void GetPersistencyValues(mmRunTimePersistencyValues &persistencyValues) const;
  void GetBytesFromData(std::vector<uint8_t> &data) const override;
  void SetDataFromBytes(std::vector<uint8_t> &data) override;
};

#endif