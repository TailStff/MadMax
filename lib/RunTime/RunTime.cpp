#include "RunTime.h"

namespace MadMax
{
  RunTime::RunTime(ExecutionEnv *_executionEnv, RunTimePersistencyValues data)
  {
    executionEnv = _executionEnv;
    cycle = executionEnv->GetCycle();

    // Runtime value
    value = data.value;
  }

  RunTime::~RunTime()
  {
    this->executionEnv = nullptr;
    cycle = 0;
    value = 0;
  }

  uint64_t RunTime::Evaluate(bool in)
  {
    if (in)
      value += cycle;
    return value;
  }

  void RunTime::Reset()
  {
    value = 0;
  }

  void RunTime::Reset(uint64_t value)
  {
    this->value = value;
  }

  /// @brief Get the persistency values of the object, here we just get the runtime value
  /// @param persistencyValues Object that will receive persistency values
  /// @return true if we have an error during persistency values retrieval, false otherwise
  void RunTime::GetPersistencyValues(RunTimePersistencyValues &persistencyValues) const
  {
    persistencyValues.value = this->value;
  }

  /// @brief Get the bytes vector that represent the object persistency values,
  /// @param data Reference to the vector that will receive the bytes that represent the object persistency values
  void RunTime::GetBytesFromData(std::vector<uint8_t> &data) const
  {
    RunTimePersistencyValues persistencyValues;
    GetPersistencyValues(persistencyValues);

    size_t size = sizeof(persistencyValues.value);
    data.resize(size);
    memcpy(data.data(), &persistencyValues.value, size);

    return;
  }

  void RunTime::SetDataFromBytes(std::vector<uint8_t> &data)
  {
  }
}