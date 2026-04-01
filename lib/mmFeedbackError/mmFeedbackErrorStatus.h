#ifndef FEEDBACKERRORSTATUS_H
#define FEEDBACKERRORSTATUS_H

#include <stdio.h>
#include "mmFeedbackErrorOption.h"
#include "mmFeedbackErrorState.h"

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