#pragma once

//#include "ESP32S_Pins.h"

#include "RTClib.h"

extern RTC_DS3231 rtc;

extern const char* ntpServerUrl;
extern const long gmtOffset_sec;
extern const int daylightOffset_sec;

bool setupRtcClock();

void rtcAdjustTime(struct tm timeinfo, bool gotTimeInfo);
