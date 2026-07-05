#include "Wifi_Api.h"

const char* wifi_ssid = "Nash_Wifi";
const char* wifi_password = "Groz1986";

bool connectToWifi(void (*callback)(const char*, bool, int)) {
  WiFi.begin(wifi_ssid, wifi_password);

  Serial.print("Wifi connecting");
  
  if (callback)
    callback("Wifi\nconnecting", true, 2);

  for (int i = 0; i < 20; i++) {
    
    delay(500);
    Serial.print(".");

    if (callback)
      callback(".", false, 2);

    if (WiFi.status() == WL_CONNECTED)
      break;
  }

  bool connected = WiFi.status() == WL_CONNECTED;

  if (connected) {
    
    Serial.println("\nWiFi connected");
    
    if (callback) callback("WiFi\nconnected", true, 2);
  }
  else
  {
    Serial.println("\nWifi connection failed.");
    
    if (callback) callback("Wifi\nconnection\nfailed.", true, 2);
  }
  
  return connected;
}
