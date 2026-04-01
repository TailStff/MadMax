#include "mmRunTime.h"

namespace MadMax
{
  mmRunTime::mmRunTime(ExecutionEnv *_executionEnv, mmRunTimePersistencyValues data)
  {
    executionEnv = _executionEnv;
    cycle = executionEnv->GetCycle();

    // Runtime value
    value = data.value;
  }

  mmRunTime::~mmRunTime()
  {
    this->executionEnv = nullptr;
    cycle = 0;
    value = 0;
  }

  uint64_t mmRunTime::Evaluate(bool in)
  {
    if (in)
      value += cycle;
    return value;
  }

  void mmRunTime::Reset()
  {
    value = 0;
  }

  void mmRunTime::Reset(uint64_t value)
  {
    this->value = value;
  }

  /// @brief Get the persistency values of the object, here we just get the runtime value
  /// @param persistencyValues Object that will receive persistency values
  /// @return true if we have an error during persistency values retrieval, false otherwise
  void mmRunTime::GetPersistencyValues(mmRunTimePersistencyValues &persistencyValues) const
  {
    persistencyValues.value = this->value;
  }

  /// @brief Get the bytes vector that represent the object persistency values,
  /// @param data Reference to the vector that will receive the bytes that represent the object persistency values
  void mmRunTime::GetBytesFromData(std::vector<uint8_t> &data) const
  {
    mmRunTimePersistencyValues persistencyValues;
    GetPersistencyValues(persistencyValues);

    size_t size = sizeof(persistencyValues.value);
    data.resize(size);
    memcpy(data.data(), &persistencyValues.value, size);

    return;
  }

  void mmRunTime::SetDataFromBytes(std::vector<uint8_t> &data)
  {
  }
}