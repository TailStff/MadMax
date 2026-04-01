#include "mmFeedbackError.h"

namespace MadMax
{
  mmFeedbackError::mmFeedbackError(ExecutionEnv *_executionEnv) : executionEnv(_executionEnv), memInput(false), memFeedback(false), memReset(false)
  {
    delayOnOff = new mmDelayOnOff(executionEnv, false);
    status.value = false;
  }

  mmFeedbackError::~mmFeedbackError()
  {
    delete delayOnOff;
  }

  bool mmFeedbackError::Evaluate(bool input, bool feedback, uint32_t onDelayFeedbackError, uint32_t offDelayFeedbackError, bool reset, FeedbackErrorOption option, FeedbackErrorStatus *status)
  {
    this->status.command = input;
    this->status.feedback = feedback;
    this->status.onDelayFeedbackError = onDelayFeedbackError;
    this->status.offDelayFeedbackError = offDelayFeedbackError;
    this->status.option = option;

    // If we just changed the input value or feedback value, we need to reset the delayOnOff state to reset the timers
    if (input != memInput || feedback != memFeedback || reset && !memReset)
      delayOnOff->EmergencyOff();

    bool feedbackMismatch = input != feedback;

    // we are already in good state, no need to evaluate the timers
    if (!feedbackMismatch)
    {
      this->status.value = false;
      this->status.state = FeedbackErrorState::Normal;
      this->status.remainingTime = -1;
    }
    else
    {
      if ((option == FeedbackErrorOption::OnlyOn && input) ||
          (option == FeedbackErrorOption::OnlyOff && !input) ||
          (option == FeedbackErrorOption::Both))
      {
        uint32_t delay = input ? onDelayFeedbackError : offDelayFeedbackError;

        this->status.value = delayOnOff->Evaluate(feedbackMismatch, delay, 0);
        this->status.state = this->status.value ? FeedbackErrorState::Error : FeedbackErrorState::Transition;
        this->status.remainingTime = delayOnOff->GetRemainingTime();
      }
      else
      {
        this->status.value = false;
        this->status.state = FeedbackErrorState::Normal;
        this->status.remainingTime = -1;
      }
    }

    if (status)
    {
      status->value = this->status.value;
      status->state = this->status.state;
      status->remainingTime = this->status.remainingTime;
    }

    memInput = input;
    memFeedback = feedback;
    memReset = reset;

    return this->status.value;
  }

  void mmFeedbackError::Reset()
  {
    delayOnOff->EmergencyOff();
    this->status.value = false;
    this->status.state = FeedbackErrorState::Normal;
    this->status.remainingTime = -1;
  }

  bool mmFeedbackError::GetValue() const
  {
    return this->status.value;
  }

  const FeedbackErrorStatus &mmFeedbackError::GetStatus() const
  {
    return this->status;
  }

#pragma region IPersistable
  /// @brief Get the bytes vector that represent the object persistency values, here we just serialize all pumps runtimes and start counts in a byte vector
  /// @param data Reference to the vector that will receive the bytes that represent the object persistency values
  void mmFeedbackError::GetBytesFromData(std::vector<uint8_t> &data) const
  {
  }

  void mmFeedbackError::SetDataFromBytes(std::vector<uint8_t> &data)
  {
  }
#pragma endregion IPersistable
}