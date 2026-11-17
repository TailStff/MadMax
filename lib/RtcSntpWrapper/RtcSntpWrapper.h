#ifndef RTCSNTPWRAPPER_H
#define RTCSNTPWRAPPER_H

#include <atomic>
#include <RTClib.h>
#include <time.h>
#include "mmLogger.h"

class RtcSntpWrapper
{

private:
  TwoWire *i2cbus;
  RTC_DS1307 rtc;

  /// @brief Flag indicating whether the RTC has been successfully initialized
  bool rtcInitialized = false;

  /// @brief Flag indicating whether the RTC date/time needs to be saved
  /// @remark This flag is set to true when the RTC date/time is updated and needs to be saved to the RTC device. It is set to false after the date/time has been successfully saved.
  /// @remark This is atomic boolean to ensure thread-safe access in a multi-threaded environment.
  static std::atomic_bool rtcNeedToSaveRTC;

  const char *ntpServer = "pool.ntp.org";
  long gmtOffset_sec = 3600;     // France
  int daylightOffset_sec = 3600; // DST

  /// @brief Gets the current time from an NTP server, with a specified timeout
  /// @param timeinfo Reference to store the retrieved time
  /// @param timeoutMs Timeout in milliseconds
  /// @return true if successful, false otherwise
  bool getNtpTime(struct tm &timeinfo, uint32_t timeoutMs = 5000);

  static void time_sync_notification_cb(struct timeval *tv);

public:
  RtcSntpWrapper(TwoWire *i2cbus);
  ~RtcSntpWrapper();

  void begin();

  void asynchroneSyncDateTimeFromNTP();
  void synchroneSyncDateTimeFromNTP();

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