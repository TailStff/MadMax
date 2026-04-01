#ifndef MADMAXPID_H
#define MADMAXPID_H

#include <cstdio>

#include "ExecutionEnv.h"

namespace MadMax
{
#define PID_STOP 0
#define PID_AUTO 1
#define PID_PAUSE 2

  class PID
  {

  private:
    // private value of cycle time (in s)
    double cycle;

    double err, prev_err, prop, integ, diff, prev_meas;
    double lim_min_integ, lim_max_integ;
    double tau;

    double coefI, coefD;

    double value;

    ExecutionEnv *executionEnv;

  public:
    // Constructors
    PID(ExecutionEnv *_executionEnv);
    ~PID();

    double Evaluate(unsigned char mode, double setpoint, double measure, double kp, double ki, double kd, double minOutput, double maxOutput, double stopValue);
  };
}

#endif