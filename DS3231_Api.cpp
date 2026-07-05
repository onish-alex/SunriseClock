#include "DS3231_Api.h" 

const char* ntpServerUrl = "pool.ntp.org";
const long gmtOffset_sec = 2 * 3600;
const int daylightOffset_sec = 3600;

RTC_DS3231 rtc;

bool setupRtcClock() {

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    return false;
  }

  rtc.disable32K();

  return true;
}

//if succeeded to get time from ntp, set it, otherwise set sketch compile time
void rtcAdjustTime(struct tm timeinfo, bool gotTimeInfo) {
  rtc.adjust(gotTimeInfo 
    ? DateTime(timeinfo.tm_year - 100, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec + 1) 
    : DateTime(F(__DATE__), F(__TIME__)));
}
