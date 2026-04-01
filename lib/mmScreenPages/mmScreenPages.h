#ifndef MMSCREENPAGES_H
#define MMSCREENPAGES_H

#include <stdio.h>
#include <HardwareSerial.h>
#include "ExecutionEnv.h"

class mmScreenPages
{

private:
    // Tick number memorized
    int64_t tickNumber;
    // Cycle duration in ms
    uint16_t cycle;

    uint8_t maxPages;

    // Memorized input (for edge detection)
    bool memIn;

    bool memLongPress;

    //
    int64_t tickPressed;

    //
    int64_t tickReleased;

    bool longPress;
    bool longPressTrigger, shortPressTrigger;

    int8_t page;

    // Execution Environment Object
    ExecutionEnv *executionEnv;

    // Pointer to access to callback function
    std::function<void()> cbShortPress;

    // Pointer to access to callback function
    std::function<void()> cbLongPress;

public:
    // Constructors
    mmScreenPages(ExecutionEnv *executionEnv, int maxPages, std::function<void()> cbShortPress, std::function<void()> cbLongPress);
    ~mmScreenPages();

    void Evaluate(bool in);
    int GetPage();
    int IncPage();
};

#endif