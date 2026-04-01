#include "mmScreenPages.h"

// Value in ms of the long press delay
#define LONGPRESSDELAY 1000L

mmScreenPages::mmScreenPages(ExecutionEnv *executionEnv, int maxPages, std::function<void()> cbShortPress, std::function<void()> cbLongPress) : memIn(false), tickPressed(0), tickReleased(0), longPress(false), memLongPress(false), page(0)
{
    this->executionEnv = executionEnv;
    this->cycle = executionEnv->GetCycle();
    this->maxPages = maxPages;

    this->cbShortPress = cbShortPress;
    this->cbLongPress = cbLongPress;
}

void mmScreenPages::Evaluate(bool in)
{
    shortPressTrigger = false;

    int64_t _tickNumber = executionEnv->GetTicks();

    if (in && !memIn)
    {
        // button was just pressed
        tickPressed = _tickNumber;
    }

    if (in && !longPress)
    {
        int64_t delay = (_tickNumber - tickPressed) * cycle;
        if (delay >= LONGPRESSDELAY)
        {
            longPress = true;
        }
    }

    if (!in && memIn)
    {
        // button was just released
        tickReleased = _tickNumber;

        if (!longPress)
            shortPressTrigger = true;
    }

    if (!in)
        longPress = false;

    longPressTrigger = longPress && !memLongPress;

    // Here it's safe to interpret shortPressTrigger & longPressTrigger
    if (shortPressTrigger && cbShortPress != nullptr)
        cbShortPress();

    if (longPressTrigger && cbLongPress != nullptr)
        cbLongPress();

    memIn = in;
    memLongPress = longPress;
}

int mmScreenPages::GetPage()
{
    return page;
}

int mmScreenPages::IncPage()
{
    page = (++page) % maxPages;
    return page;
}