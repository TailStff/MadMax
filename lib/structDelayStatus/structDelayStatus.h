#ifndef DELAYSTATUS_H
#define DELAYSTATUS_H

#include <stdio.h>

struct DelayStatus
{
  int64_t remainingTime;
  bool value;
};

#endif