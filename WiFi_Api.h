#pragma once

#include <WiFi.h>

extern const char* wifi_ssid;
extern const char* wifi_password;

bool connectToWifi(void (*callback)(const char*, bool, int));
