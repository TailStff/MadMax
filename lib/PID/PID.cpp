// Based on BanaanKiamanesh PID anti windup : https://github.com/BanaanKiamanesh/Anti-Windup-PID-Controller/blob/main/PID.h

#include "PID.h"

namespace MadMax
{
  
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

  PID::PID(ExecutionEnv *_executionEnv)
  {
    executionEnv = _executionEnv;

    cycle = (double)(_executionEnv->GetCycle()) * 0.001;

    tau = 0.02;

    coefI = 0.5;
    coefD = 2.0;

    prop = 0.0;
    integ = 0.0;
    diff = 0.0;

    prev_err = 0.0;
    prev_meas = 0.0;
    value = 0.0;
  }

  PID::~PID()
  {
  }

  /** madmax PID */
  // Mode : 0 : Stop/Manual, 1 : Automatic, 2 : Pause
  double PID::Evaluate(unsigned char mode, double setpoint, double measure, double kp, double ki, double kd, double minOutput, double maxOutput, double stopValue)
  {

    switch (mode)
    {

    case PID_STOP:
      // Stop/Manual

      prop = 0.0;
      integ = 0.0;
      diff = 0.0;

      prev_err = 0.0;
      prev_meas = 0.0;
      value = stopValue;

      break;

    case PID_AUTO:
      // Automatic
      {
        // Error Signal
        err = setpoint - measure;

        // Proportional Term
        prop = kp * err;

        // Integral Term
        integ += coefI * ki * cycle * (err + prev_err);

        // Anti-wind-up via Dynamic Integrator Clamping
        // Limits Computationa and Application
        if (maxOutput > prop)
          lim_max_integ = maxOutput - prop;
        else
          lim_max_integ = 0.0;

        if (minOutput < prop)
          lim_min_integ = minOutput - prop;
        else
          lim_min_integ = 0.0;

        // Constrain Integrator
        if (integ > lim_max_integ)
          integ = lim_max_integ;

        else if (integ < lim_min_integ)
          integ = lim_min_integ;

        // Derivative (Band - Limited Differentiator)
        /**** Derivative on Measurements ****/
        diff = (coefD * kd * (measure - prev_meas) + (coefD * tau - cycle) * diff) / (coefD * tau + cycle);

        // Calculate Output and Apply Limits
        value = prop + integ + diff;

        // Constrain Value in Given Bounds
        value = constrain(value, minOutput, maxOutput);

        // Store error and Measurement for Later Use
        prev_err = err;
        prev_meas = measure;

        return value;
      }
      break;

    case PID_PAUSE:
      // Pause
      break;
    }

    return value;
  }
}