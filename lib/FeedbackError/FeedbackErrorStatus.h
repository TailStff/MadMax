#ifndef MADMAXFEEDBACKERRORSTATUS_H
#define MADMAXFEEDBACKERRORSTATUS_H

#include <cstdio>
#include "FeedbackErrorOption.h"
#include "FeedbackErrorState.h"

namespace MadMax
{
    struct FeedbackErrorStatus
    {
        bool command;
        bool feedback;
        bool value;
        uint32_t onDelayFeedbackError;
        uint32_t offDelayFeedbackError;
        FeedbackErrorOption option;
        int64_t remainingTime;
        FeedbackErrorState state;
    };
}

#endif