#include <Arduino.h>
#include "IntervalCallback.h"

#include "millis64.h"

IntervalCallback::IntervalCallback(void (*callbackFunction)(void)) {
  voidCallback = callbackFunction;
  _automaticRestart = false;
  previousMillis = 0;
}

void IntervalCallback::Tick() {
  if (!active)
    return;

  uint64_t currentMillis = Millis64::millis64();

  if (currentMillis - previousMillis >= _interval) {

    LastExecutionInterval = currentMillis - previousMillis;

    previousMillis += (LastExecutionInterval/_interval) * _interval;

    // unset interval to not execute more than one time
    active = false;

    voidCallback();
    if (_automaticRestart) Start(_interval, _automaticRestart);
  }
}

void IntervalCallback::Start(long interval, bool automaticRestart) {

  _interval = interval;
  active = true;
  _automaticRestart = automaticRestart;
  if (previousMillis == 0)
    previousMillis = Millis64::millis64();
}

void IntervalCallback::Stop() {
  active = false;
  _automaticRestart = false;
}
