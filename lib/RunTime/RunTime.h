#pragma once

#ifndef MADMAXRUNTIME_H
#define MADMAXRUNTIME_H

#include <stdio.h>
#include <HardwareSerial.h>

#include "IPrimitive.h"
#include "ISerializable.h"
#include "IPrimitiveTyped.h"
#include "RunTimeStatus.h"

#include "ExecutionEnv.h"

namespace MadMax
{
  struct __attribute__((packed)) RunTimePersistencyValues
  {
    uint64_t value;
  };

  class RunTime : public IPrimitiveTyped<RunTimePersistencyValues>, public IPrimitive, public ISerializable
  {

  private:
    uint16_t cycleTimeSpan;
    ExecutionEnv *executionEnv;

    RunTimeStatus status;

  public:
    // Constructors
    RunTime(ExecutionEnv *_executionEnv, RunTimePersistencyValues data = {.value = 0});
    ~RunTime();

    uint64_t Evaluate(bool in);

    const RunTimeStatus &GetStatus() const;

    void Reset(uint64_t value = 0);
    uint64_t GetValue() { return status.value; }

    /// @brief Static function that give the size of the serialized data of the object, here we just serialize the uint64_t value
    /// @return Size in bytes of the serialized data of the object
    static size_t GetSerializedSize()
    {
      return sizeof(RunTimePersistencyValues);
    }

    void GetPersistencyValues(RunTimePersistencyValues &persistencyValues) const;
    void GetBytesFromData(std::vector<uint8_t> &data) const override;
    void SetDataFromBytes(std::vector<uint8_t> &data) override;

    const ISerializable *AsSerializable() const override { return this; }
    ISerializable *AsSerializable() override { return this; }

    const RunTime *AsRunTime() const override { return this; }
    RunTime *AsRunTime() override { return this; }
  };
}

#endif