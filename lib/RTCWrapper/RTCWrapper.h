#ifndef RTCWRAPPER_H
#define RTCWRAPPER_H

#include <RTClib.h>
#include <time.h>
#include "mmLogger.h"

class RTCWrapper
{

private:
  TwoWire *i2cbus;
  RTC_DS1307 rtc;

  bool rtcInitialized = false;

  const char *ntpServer = "pool.ntp.org";
  long gmtOffset_sec = 3600;     // France
  int daylightOffset_sec = 3600; // DST

  /// @brief Calculates the day of the week for a given date
  /// @param year 
  /// @param month 
  /// @param day 
  /// @return The day of the week (0 = Sunday, 1 = Monday, etc.)
  uint8_t getDayOfWeek(uint16_t year, uint8_t month, uint8_t day);

  /// @brief Gets the current time from an NTP server, with a specified timeout
  /// @param timeinfo Reference to store the retrieved time
  /// @param timeoutMs Timeout in milliseconds
  /// @return true if successful, false otherwise
  bool getNtpTime(struct tm &timeinfo, uint32_t timeoutMs = 5000);

public:
  RTCWrapper(TwoWire *i2cbus);
  ~RTCWrapper();

  void begin();

  void SyncDateTimeFromNTP();

  /// @brief Gets the current date and time from the RTC
  /// @param now Reference to store the retrieved date and time
  /// @return true if successful, false otherwise
  bool RTCGet(DateTime &now);

  /// @brief Sets the date and time on the RTC
  /// @param dateTime Reference to the date and time to set
  /// @return true if successful, false otherwise
  bool RTCSet(DateTime &dateTime);
};

#endif