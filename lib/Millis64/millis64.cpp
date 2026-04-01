
#include "millis64.h"
#include "esp_timer.h"

namespace Millis64
{
  unsigned long long int millis64()
  {
    return (unsigned long long int)(esp_timer_get_time() / 1000);
  }
}
