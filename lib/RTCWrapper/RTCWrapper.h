#ifndef RTCWRAPPER_H
#define RTCWRAPPER_H

#include <RTClib.h>
#include <time.h>

class RTCWrapper {

private:

  TwoWire* i2cbus;
  RTC_DS1307 rtc;

  const char* ntpServer = "pool.ntp.org";
  long gmtOffset_sec = 3600;      // France
  int daylightOffset_sec = 3600;  // DST

  uint8_t getDayOfWeek(uint16_t year, uint8_t month, uint8_t day);

public:

  RTCWrapper(TwoWire* i2cbus);
  ~RTCWrapper();

  void begin();
  
  void SyncDateTimeFromNTP();

  void RTCGet(DateTime& now);
  /// Function that SET Date/Time ///
  ///
  bool uRTCSet(DateTime& dateTime);
  bool getNtpTime(struct tm& timeinfo, uint32_t timeoutMs = 5000);
};

#endif