#ifndef MADMAXFEEDBACKERROR_H
#define MADMAXFEEDBACKERROR_H

#include <cstdio>
#include <unordered_map>
#include <memory>

#include "IPrimitive.h"
#include "ExecutionEnv.h"
#include "FeedbackErrorOption.h"
#include "FeedbackErrorState.h"
#include "FeedbackErrorStatus.h"
#include "DelayOnOff.h"

namespace MadMax
{
  class FeedbackError : public IPrimitive
  {
  private:
    FeedbackErrorStatus status;

    ExecutionEnv *executionEnv;

    DelayOnOff *delayOnOff;
    bool memInput, memFeedback, memReset;

  public:
    // Constructors
    FeedbackError(ExecutionEnv *_executionEnv);
    ~FeedbackError();

    /**
     * @brief Evaluates a feedback mismatch condition with configurable delay timers.
     *
     * This function detects inconsistencies between a command input and its corresponding
     * feedback signal. When a mismatch occurs, a delay timer is applied before reporting
     * an error. The delay used depends on the transition direction (ON or OFF) and the
     * configured evaluation option.
     *
     * If the input or feedback state changes, or if a reset rising edge is detected,
     * the internal delay timer is reset to avoid false error detection during transitions.
     *
     * The function updates the internal state of the object and optionally fills the
     * provided status structure with detailed information about the current evaluation.
     *
     * @param invalue Current command or control input state.
     * @param feedback Current feedback state from the controlled device.
     * @param onDelayFeedbackError Delay (in milliseconds) before reporting an error when the input is ON and the feedback does not match.
     * @param offDelayFeedbackError Delay (in milliseconds) before reporting an error when the input is OFF and the feedback does not match.
     * @param reset When a rising edge is detected on this signal, the internal delay timers are reset.
     * @param option Defines in which situations feedback errors should be evaluated (OnlyOn, OnlyOff, or Both).
     * @param status Optional pointer to a structure that receives detailed evaluation results including error state and remaining timer value.
     * @return true if a feedback error is detected after the configured delay, false otherwise.
     */
    bool Evaluate(bool invalue, bool feedback, uint32_t onDelayFeedbackError, uint32_t offDelayFeedbackError, bool reset, FeedbackErrorOption option = FeedbackErrorOption::OnlyOn, FeedbackErrorStatus *status = nullptr);

    /**
     * @brief Resets the internal state of the feedback error detector.
     *
     * This function immediately clears any active or pending feedback error
     * condition. The internal delay timer is stopped and the state is returned
     * to normal.
     *
     * After calling this function:
     *
     * - The feedback error output is cleared.
     *
     * - The internal timer used for mismatch detection is reset.
     *
     * - The remaining time indicator is set to -1.
     *
     * This function can be used to acknowledge an error or to reinitialize
     * the supervision logic when the system state changes.
     */
    void Reset();

    bool GetValue() const;
    const FeedbackErrorStatus &GetStatus() const;
  };
}

#endif