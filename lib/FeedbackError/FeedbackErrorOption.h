#ifndef MADMAXFEEDBACKERROROPTION_H
#define MADMAXFEEDBACKERROROPTION_H

namespace MadMax
{
    enum class FeedbackErrorOption
    {
        // Normal state, no feedback error
        OnlyOn = 0,
        // We have a feedback error, but we are still in the delay time, so we are in transition state
        OnlyOff = 1,
        // We have a feedback error, and we are out of the delay time, so we are in error state
        Both = 2
    };
}

#endif