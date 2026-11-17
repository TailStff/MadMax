#include "RtcSntpWrapper.h"
#include <esp_netif.h>
#include <esp_sntp.h>

std::atomic_bool RtcSntpWrapper::rtcNeedToSaveRTC{false};

/// @brief Function to set the time zone based on the provided offset and daylight saving time (From esp32-hal-time.c)
/// @param offset The offset in seconds from UTC (e.g., 3600 for UTC+1).
/// @param daylight The daylight saving time offset in seconds (e.g., 3600 for 1 hour).
static void setTimeZone(long offset, int daylight)
{
  char cst[17] = {0};
  char cdt[17] = "DST";
  char tz[33] = {0};

  if (offset % 3600)
  {
    sprintf(cst, "UTC%ld:%02u:%02u", offset / 3600, abs((offset % 3600) / 60), abs(offset % 60));
  }
  else
  {
    sprintf(cst, "UTC%ld", offset / 3600);
  }
  if (daylight != 3600)
  {
    long tz_dst = offset - daylight;
    if (tz_dst % 3600)
    {
      sprintf(cdt, "DST%ld:%02u:%02u", tz_dst / 3600, abs((tz_dst % 3600) / 60), abs(tz_dst % 60));
    }
    else
    {
      sprintf(cdt, "DST%ld", tz_dst / 3600);
    }
  }
  sprintf(tz, "%s%s", cst, cdt);
  setenv("TZ", tz, 1);
  tzset();
}

/// @brief Static callback function for time synchronization events from SNTP. This function is called when the system time is synchronized with an NTP server.
/// @param tv
void RtcSntpWrapper::time_sync_notification_cb(struct timeval *tv)
{
  (void)tv;
  MM_LOG_INFO("RTCWrapper", "RTC notification of a time synchronization event");
  rtcNeedToSaveRTC.store(true, std::memory_order_release);
}

// Constructor
RtcSntpWrapper::RtcSntpWrapper(TwoWire *i2cbus)
{
  this->i2cbus = i2cbus;

  rtcNeedToSaveRTC.store(false, std::memory_order_release);
}

// Initialization function
void RtcSntpWrapper::begin()
{
  this->rtcInitialized = false;

  if (!this->rtc.begin(i2cbus))
  {
    MM_LOG_ERROR("RTCWrapper", "RTC not detected");
  }
  else
  {
    MM_LOG_TRACE("RTCWrapper", "RTC detected");

    // We prepare sntp to get the time from NTP servers and set the timezone
    esp_netif_init();
    if (sntp_enabled())
    {
      sntp_stop();
    }
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, (char *)this->ntpServer);
    sntp_setservername(1, nullptr);
    sntp_setservername(2, nullptr);

    sntp_set_sync_interval(2 * 3600 * 1000); // 2 * 3600 * 1000 = 2 hours
    sntp_set_time_sync_notification_cb(&RtcSntpWrapper::time_sync_notification_cb);

    sntp_init();

    setTimeZone(-this->gmtOffset_sec, this->daylightOffset_sec);

    this->rtcInitialized = true;
  }
}

// Destructor
RtcSntpWrapper::~RtcSntpWrapper()
{
}

/// @brief Asynchronously synchronize the RTC date/time from NTP servers. This function checks if the RTC is initialized and if a synchronization is needed, then retrieves the current time from NTP servers and updates the RTC accordingly.
/// @remark This function is non-blocking and returns immediately. The actual synchronization may take some time, depending on network conditions and NTP server response times.
void RtcSntpWrapper::asynchroneSyncDateTimeFromNTP()
{
  // Check if the RTC is initialized
  if (!this->rtcInitialized)
    return;

  if (!rtcNeedToSaveRTC.exchange(false, std::memory_order_seq_cst)) // read and reset the flag atomically
    return;

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 50)) // wait 50 ms to get the time from NTP servers
  {
    MM_LOG_WARN("RTCWrapper", "RTC date/time could not be saved because getting time from NTP servers failed");
    rtcNeedToSaveRTC.store(true, std::memory_order_release);
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

  MM_LOG_INFO("RTCWrapper", "Synchronization from NTP servers successfully done");
}

void RtcSntpWrapper::synchroneSyncDateTimeFromNTP()
{
  // Check if the RTC is initialized
  if (!this->rtcInitialized)
    return;

  struct tm timeinfo;

  if (!getNtpTime(timeinfo))
  {
    MM_LOG_ERROR("RtcSntpWrapper", "Getting time from NTP servers failed");
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

  MM_LOG_TRACE("RtcSntpWrapper", "Synchronization from NTP servers successfully done");
}

/// Function that SET Date/Time ///
///
bool RtcSntpWrapper::RTCSet(DateTime &dateTime)
{
  // Check if the RTC is initialized
  if (!this->rtcInitialized)
    return false;

  this->rtc.adjust(dateTime);

  MM_LOG_TRACE("RtcSntpWrapper", "RTC date/time set successfully");

  return true;
}

bool RtcSntpWrapper::RTCGet(DateTime &now)
{
  // Check if the RTC is initialized
  if (!this->rtcInitialized)
    return false;

  now = this->rtc.now();

  return true;
}

bool RtcSntpWrapper::getNtpTime(struct tm &timeinfo, uint32_t timeoutMs)
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
