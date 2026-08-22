#include "RTCWrapper.h"

uint8_t RTCWrapper::getDayOfWeek(uint16_t year, uint8_t month, uint8_t day)
{
  static int t[] = {0, 3, 2, 5, 3, 0, 5, 1, 4, 6, 2, 4};
  if (month < 3)
  {
    year -= 1;
  }
  return (year + year / 4 - year / 400 + year / 100 + t[month - 1] + day) % 7;
}

// Constructor
RTCWrapper::RTCWrapper(TwoWire *i2cbus)
{
  this->i2cbus = i2cbus;
}

// Initialization function
void RTCWrapper::begin()
{
  this->rtcInitialized = false;

  if (!this->rtc.begin(i2cbus))
  {
    MM_LOG_ERROR("RTCWrapper", "RTC not detected");
  }
  else
  {
    this->rtcInitialized = true;
    MM_LOG_TRACE("RTCWrapper", "RTC detected");
    configTime(this->gmtOffset_sec, this->daylightOffset_sec, this->ntpServer);
  }
}

// Destructor
RTCWrapper::~RTCWrapper()
{
}

void RTCWrapper::SyncDateTimeFromNTP()
{
  if (!this->rtcInitialized)
  {
    MM_LOG_WARN("RTCWrapper", "Synchronization from NTP servers could not be done");
    return;
  }

  struct tm timeinfo;

  if (!getNtpTime(timeinfo))
  {
    MM_LOG_ERROR("RTCWrapper", "Getting time from NTP servers failed");
    return;
  }

  DateTime ntpTime(
      timeinfo.tm_year + 1900,
      timeinfo.tm_mon + 1,
      timeinfo.tm_mday,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec);

  rtc.adjust(ntpTime);

  MM_LOG_TRACE("RTCWrapper", "Synchronization from NTP servers successfully done");
}

/// Function that SET Date/Time ///
///
bool RTCWrapper::RTCSet(DateTime &dateTime)
{
  if (!this->rtcInitialized)
  {
    MM_LOG_WARN("RTCWrapper", "Setting RTC date/time could not be done");
    return false;
  }

  this->rtc.adjust(dateTime);

  MM_LOG_TRACE("RTCWrapper", "RTC date/time set successfully");

  return true;
}

bool RTCWrapper::RTCGet(DateTime &now)
{
  if (!this->rtcInitialized)
  {
    MM_LOG_WARN("RTCWrapper", "Getting RTC date/time could not be done");
    return false;
  }

  now = this->rtc.now();

  MM_LOG_TRACE("RTCWrapper", "RTC date/time retrieved successfully");

  return true;
}

bool RTCWrapper::getNtpTime(struct tm &timeinfo, uint32_t timeoutMs)
{
  uint32_t start = millis();

  while (millis() - start < timeoutMs)
  {
    if (getLocalTime(&timeinfo))
    {
      return true;
    }
    delay(500);
  }
  return false;
}
