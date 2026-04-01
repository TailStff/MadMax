#ifndef STRUCTFAULTS_H
#define STRUCTFAULTS_H

#include <cstdio>

namespace MadMax
{
  struct Faults
  {
    bool fault;
    bool ackFault;
    bool criticalFault;
  };
}
#endif
