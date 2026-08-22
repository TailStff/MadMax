#ifndef MADMAXDELAYONOFF_H
#define MADMAXDELAYONOFF_H

#include <cstdint>

#include "IPrimitive.h"
#include "ExecutionEnv.h"
#include "DelayOnOffStatus.h"

namespace MadMax
{
  class DelayOnOff : public IPrimitive
  {
  private:
    int64_t tickNumber;
    uint16_t cycle;

    ExecutionEnv *executionEnv;

    DelayOnOffStatus status;

  public:
    // Constructors
    DelayOnOff(ExecutionEnv *_executionEnv, bool initialValue = false);
    ~DelayOnOff();

    /// @brief Evaluates the delay on/off state based on the input and delay parameters.
    /// This function checks the current input state and compares it with the previous output state.
    /// If the input state has changed, it starts a timer based on the specified delayOn or delayOff values.
    /// Once the elapsed time exceeds the specified delay, the output state is updated to match the input state.
    /// The function also updates the status structure with the current input, output, delay values, remaining time, and elapsed time.
    /// @param input The current input state (true for ON, false for OFF)
    /// @param delayOn The delay time in milliseconds to wait before turning ON the output when the input is ON.
    /// @param delayOff The delay time in milliseconds to wait before turning OFF the output when the input is OFF.
    /// @param status Optional pointer to a DelayOnOffStatus structure to receive the current status
    /// @return The current output state (true for ON, false for OFF)
    bool Evaluate(bool input, uint32_t delayOn, uint32_t delayOff, DelayOnOffStatus *status = nullptr);

    /// @brief Immediately sets the output state to ON and resets the internal timer.
    void EmergencyOn();

    /// @brief Immediately sets the output state to OFF and resets the internal timer.
    void EmergencyOff();

    const bool GetValue() const { return this->status.output; };
    const DelayOnOffStatus GetStatus() const { return this->status; };
  };
}

#endif