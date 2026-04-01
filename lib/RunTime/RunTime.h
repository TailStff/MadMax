#ifndef MADMAXRUNTIME_H
#define MADMAXRUNTIME_H

#include <stdio.h>
#include <HardwareSerial.h>
#include "IPrimitive.h"
#include "ExecutionEnv.h"

namespace MadMax
{
  struct __attribute__((packed)) RunTimePersistencyValues
  {
    uint64_t value;
  };

  class RunTime : public IPrimitiveTyped<RunTimePersistencyValues>, public IPrimitive
  {

  private:
    uint16_t cycle;
    uint64_t value;
    ExecutionEnv *executionEnv;

  public:
    // Constructors
    RunTime(ExecutionEnv *_executionEnv, RunTimePersistencyValues data = {.value = 0});
    ~RunTime();

    uint64_t Evaluate(bool in);
    void Reset();
    void Reset(uint64_t value);
    uint64_t GetValue() { return value; }

    /// @brief Static function that give the size of the serialized data of the object, here we just serialize the uint64_t value
    /// @return Size in bytes of the serialized data of the object
    static size_t GetSerializedSize()
    {
      return sizeof(RunTimePersistencyValues);
    }

    void GetPersistencyValues(RunTimePersistencyValues &persistencyValues) const;
    void GetBytesFromData(std::vector<uint8_t> &data) const override;
    void SetDataFromBytes(std::vector<uint8_t> &data) override;
  };
}

#endif