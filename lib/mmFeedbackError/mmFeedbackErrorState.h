#ifndef FEEDBACKERRORSTATE_H
#define FEEDBACKERRORSTATE_H

namespace MadMax
{
    enum class FeedbackErrorState
    {
        // Normal state, no feedback error
        Normal = 0,
        // We have a feedback error, but we are still in the delay time, so we are in transition state
        Transition = 1,
        // We have a feedback error, and we are out of the delay time, so we are in error state
        Error = 2
    };
}

#endif