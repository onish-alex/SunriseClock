#include "DS3231_Api.h"
#include "SSD1306_Api.h"
#include "WiFi_Api.h"
#include <ezButton.h>
#include <Preferences.h>

#define PIN_CLOCK_INTERRUPT_SQW 2 //interrupts for RTC alarms
#define PIN_SELECT_BUTTON 16
#define PIN_MODE_BUTTON 17
#define PIN_ENCODER_DT 4
#define PIN_ENCODER_CLK 0

#define DIRECTION_CW  0
#define DIRECTION_CCW 1 

const char daysOfTheWeek[7][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const int rtcRefreshDelayMs = 250;

static const uint8_t icon_alarm[120] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙
    // ∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙
    // ∙∙∙███∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙███∙∙∙
    // ∙∙███∙∙∙█∙∙∙∙∙██∙∙∙∙∙█∙∙∙███∙∙
    // ∙∙██∙∙∙██∙∙∙∙████∙∙∙∙██∙∙∙██∙∙
    // ∙██∙∙∙██∙∙∙████████∙∙∙██∙∙∙██∙
    // ∙██∙∙██∙∙∙███∙∙∙∙███∙∙∙██∙∙██∙
    // ∙∙∙∙██∙∙∙██∙∙∙∙∙∙∙∙██∙∙∙██∙∙∙∙
    // ∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙
    // ∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙
    // ∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙
    // ∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙
    // ∙∙∙∙∙████████████████████∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙██∙∙██∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 
    0x0f, 0x00, 0x03, 0xc0, 0x1c, 0x03, 0x00, 0xe0, 
    0x38, 0x83, 0x04, 0x70, 0x31, 0x87, 0x86, 0x30, 
    0x63, 0x1f, 0xe3, 0x18, 0x66, 0x38, 0x71, 0x98, 
    0x0c, 0x60, 0x18, 0xc0, 0x00, 0xc0, 0x0c, 0x00, 
    0x01, 0xc0, 0x0e, 0x00, 0x01, 0x80, 0x06, 0x00, 
    0x01, 0x80, 0x06, 0x00, 0x01, 0x80, 0x06, 0x00, 
    0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x02, 0x00, 
    0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x03, 0x00, 
    0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 
    0x03, 0x00, 0x03, 0x00, 0x07, 0x00, 0x03, 0x80, 
    0x07, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x80, 0x00, 
    0x00, 0x0c, 0xc0, 0x00, 0x00, 0x0f, 0xc0, 0x00, 
    0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t icon_datetime[120] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙
    // ███████████████████████∙∙∙∙∙∙∙
    // ████████████████∙███████∙∙∙∙∙∙
    // █████∙∙█████████∙█∙█████∙∙∙∙∙∙
    // ████████████████████████∙∙∙∙∙∙
    // ████████████████████████∙∙∙∙∙∙
    // ████████████████████████∙∙∙∙∙∙
    // ██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙
    // ██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙
    // ██∙∙███∙███∙∙██∙∙███∙∙██∙∙∙∙∙∙
    // ██∙∙███∙███∙∙██∙∙███∙∙██∙∙∙∙∙∙
    // ██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙
    // ██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███████∙∙∙∙
    // ██∙∙███∙███∙∙██∙∙∙██∙∙∙∙∙███∙∙
    // ██∙∙███∙███∙∙██∙∙██∙∙∙█∙∙∙∙██∙
    // ██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙█∙∙∙∙∙█∙
    // ██∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙█∙∙∙∙∙█∙
    // ██∙∙███∙███∙∙██∙██∙∙∙∙██∙∙∙∙██
    // ██∙∙███∙███∙∙██∙██∙∙∙∙██∙∙∙∙██
    // ██∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙██∙∙∙██
    // ██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙██∙∙█∙
    // ███████████████████∙∙∙∙∙∙∙∙██∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙██∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████████∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x60, 0x00, 
    0x06, 0x00, 0x40, 0x00, 0x06, 0x00, 0x60, 0x00, 
    0x06, 0x00, 0x60, 0x00, 0xff, 0xff, 0xfe, 0x00, 
    0xff, 0xff, 0x7f, 0x00, 0xf9, 0xff, 0x5f, 0x00, 
    0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 
    0xff, 0xff, 0xff, 0x00, 0xc0, 0x00, 0x03, 0x00, 
    0xc0, 0x00, 0x03, 0x00, 0xce, 0xe6, 0x73, 0x00, 
    0xce, 0xe6, 0x73, 0x00, 0xc0, 0x00, 0x03, 0x00, 
    0xc0, 0x00, 0x1f, 0xc0, 0xce, 0xe6, 0x30, 0x70, 
    0xce, 0xe6, 0x62, 0x18, 0xc0, 0x00, 0x42, 0x08, 
    0xc0, 0x00, 0xc2, 0x08, 0xce, 0xe6, 0xc3, 0x0c, 
    0xce, 0xe6, 0xc3, 0x0c, 0xc0, 0x00, 0xc1, 0x8c, 
    0xc0, 0x00, 0x40, 0xc8, 0xff, 0xff, 0xe0, 0x18, 
    0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x1f, 0xe0, 
    0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t image_data_doorexiticon[120] = {
    // ∙∙∙∙██████████████████∙∙∙∙∙∙∙∙
    // ∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙███████∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙██████████∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙██████████∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙█████∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙█████∙██∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙████████∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x0f, 0xff, 0xfc, 0x00, 0x08, 0x00, 0x04, 0x00, 
    0x0c, 0x00, 0x04, 0x00, 0x0e, 0x00, 0x04, 0x00, 
    0x0f, 0x80, 0x04, 0x00, 0x0f, 0xc0, 0x04, 0x00, 
    0x0f, 0xe0, 0x04, 0x00, 0x0f, 0xf0, 0x04, 0x00, 
    0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x18, 0x00, 
    0x0f, 0xf0, 0x30, 0x00, 0x0f, 0xf0, 0x60, 0x00, 
    0x0f, 0xf0, 0xff, 0xc0, 0x0f, 0xf0, 0xff, 0xc0, 
    0x0f, 0xf0, 0x60, 0x00, 0x0f, 0xf0, 0x30, 0x00, 
    0x0f, 0xf0, 0x18, 0x00, 0x0f, 0xb0, 0x00, 0x00, 
    0x0f, 0xb0, 0x04, 0x00, 0x0f, 0xf0, 0x04, 0x00, 
    0x0f, 0xf0, 0x04, 0x00, 0x0f, 0xf0, 0x04, 0x00, 
    0x0f, 0xf0, 0x04, 0x00, 0x0f, 0xf0, 0x04, 0x00, 
    0x0f, 0xf0, 0x04, 0x00, 0x03, 0xf0, 0x00, 0x00, 
    0x00, 0xf0, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 
    0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

char buffer[64];
short encoderCLK, prevEncoderCLK;

//clock states
uint8_t prevSecond;
int lastClockCheck = 0;

Preferences prefs; //access to flash memory

//int counter = 0;
//int direction = DIRECTION_CW;

#pragma region Menu

enum class MenuItem
{
  DATE_TIME      = 0,
  SETTING_ALARM  = 1,

  FIRST = DATE_TIME,
  LAST = SETTING_ALARM
};

struct MenuItemInfo {
  const char* Name;
  MenuItem Code;
  const uint8_t* Icon;
  int x, y;
};

const MenuItemInfo menuItems[] = {
  MenuItemInfo("Date & clock", MenuItem::DATE_TIME, icon_datetime, 19, 20),
  MenuItemInfo("Setting alarm", MenuItem::SETTING_ALARM, icon_alarm, 79, 20),
};

constexpr int MENU_SIZE =
    sizeof(menuItems) / sizeof(menuItems[0]);

int currentMenuItemIndex = 0;

#pragma endregion

#pragma region AlarmMenuItem

enum class AlarmItem
{
  HOUR   = 0,
  MINUTE = 1,
  ENABLE = 2,
  RETURN = 3,

  FIRST = HOUR,
  LAST = RETURN
};

struct AlarmItemInfo {
  AlarmItem Code;
  const uint8_t* Icon;
  int x, y;
};

const AlarmItemInfo alarmItems[] = {
  AlarmItemInfo(AlarmItem::HOUR, NULL, 16, 5),
  AlarmItemInfo(AlarmItem::MINUTE, NULL, 81, 5),
  AlarmItemInfo(AlarmItem::ENABLE, NULL, 16, 36),
  AlarmItemInfo(AlarmItem::RETURN, image_data_doorexiticon, 84, 31),
};

constexpr int ALARM_MENU_SIZE =
    sizeof(alarmItems) / sizeof(alarmItems[0]);

int currentAlarmItemIndex = 0;

#pragma endregion

#pragma region Alarm stored settings

struct AlarmSettings {
  bool enabled;
  uint8_t hour;
  uint8_t minute;
  //uint8_t volume;
  //uint8_t brightness;
};

AlarmSettings alarmSettings;

int alarmMinutesTmp; //temp variable for editing alarm time
bool alarmEnabledTmp; //temp variable for editing alarm mode

bool needRedrawAlarmMenu = true; //flag if need to redraw alarm menu after any input. Frequent refreshing in loop isn't recommended as it leads to encoder glitches

#pragma endregion

bool isInMenu = false;
bool rtcInit = false;

//bool led = false;

ezButton modeButton(PIN_MODE_BUTTON, INPUT_PULLUP);
ezButton selectButton(PIN_SELECT_BUTTON);

void setup() {
  Serial.begin(115200);

  //oled screen
  setupOledScreen();
  
  //datetime
  rtcInit = setupRtcClock();
  tryInitDateTime();

  //alarm
  #pragma region alarm

  pinMode(PIN_CLOCK_INTERRUPT_SQW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_CLOCK_INTERRUPT_SQW), onAlarm, FALLING);

  // set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
  // if not done, this easily leads to problems, as both register aren't reset on reboot/recompile
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // stop oscillating signals at SQW Pin
  // otherwise setAlarm1 will fail
  rtc.writeSqwPinMode(DS3231_OFF);

  // turn off alarm 2 (in case it isn't off already)
  // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
  rtc.disableAlarm(2);

  alarmMinutesTmp = alarmSettings.hour * 60 + alarmSettings.minute; 
  alarmEnabledTmp = alarmSettings.enabled;

  //reading alarmSettings from flash memory
  prefs.begin("settings", true);
  prefs.getBytes("alarm", &alarmSettings, sizeof(alarmSettings));
  prefs.end();

  #pragma endregion

  //buttons
  modeButton.setDebounceTime(50);
  selectButton.setDebounceTime(50);

  //encoder
  pinMode(PIN_ENCODER_CLK, INPUT);
  pinMode(PIN_ENCODER_DT, INPUT);
  prevEncoderCLK = digitalRead(PIN_ENCODER_CLK);
}

void loop() {
  
  MenuItemInfo currentMenuItem = menuItems[currentMenuItemIndex];
  
  if (isInMenu) {
    
    displayClearResetPosition();
  
    displayPrintln(currentMenuItem.Name);

    for (int i = 0; i < MENU_SIZE; i++) {
      
      MenuItemInfo menuItem = menuItems[i];

      if (currentMenuItemIndex == i) {
        display.fillRect(menuItem.x, menuItem.y, 30, 30, SSD1306_WHITE);
      }

      display.drawBitmap(menuItem.x, menuItem.y, menuItem.Icon, 30, 30, currentMenuItemIndex == i ? SSD1306_BLACK : SSD1306_WHITE);
    }

    display.display();
  }
  else if (currentMenuItem.Code == MenuItem::DATE_TIME) {

    if (!rtcInit) {
      
      displayClearResetPosition();
      displayPrintln("Clock chip\nfailure!", DisplayOptions{2});
      display.display();
      return;
    }

    DateTime currentDateTime = rtc.now();

    //check if minimal refresh delay passed and if last time second has changed, in order to make screen refresh more rare and precise 
    //the less delay (lastClockCheck), the higher precision of displayed time, but more time checks in a second
    int currentMillis = millis();
    
    if (currentMillis - lastClockCheck >= rtcRefreshDelayMs) {
      
      lastClockCheck = currentMillis;
    
      if (currentDateTime.second() != prevSecond) {
        
        prevSecond = currentDateTime.second();

        displayClearResetPosition();

        getDateString(currentDateTime, buffer, sizeof(buffer));
        displayPrint(buffer);

        displayPrintln("\n");

        // Serial.printf(buffer);
        // Serial.printf("\n");

        getTimeString(currentDateTime, buffer, sizeof(buffer));
        displayPrintln(buffer, DisplayOptions{2});

        snprintf(buffer, sizeof(buffer), "%.1fc", rtc.getTemperature());
        displayPrint(buffer, DisplayOptions{2});
        
        display.setTextSize(2);
        display.write(0xF8);

        // Serial.printf(buffer);
        // Serial.printf("\n");

        display.display();
      }
    }
  }
  else if (currentMenuItem.Code == MenuItem::SETTING_ALARM && needRedrawAlarmMenu) {

    needRedrawAlarmMenu = false;

    // if (millis() - alarmMenuLastDisplayUpdate < 250)
    //   return;

    // alarmMenuLastDisplayUpdate = millis();

    displayClearResetPosition();
    
    //DateTime alarmTime = rtc.getAlarm1();
    //DateTime alarmTime = DateTime(0, 0, 0, alarmSettings.hour, alarmSettings.minute, 0);

    
    // getTimeString(currentDateTime, buffer, sizeof(buffer));
    // Serial.println(buffer);



    //debug, printing alarmTime
    // char alarmStr[12] = "DD hh:mm:ss";
    // alarmTime.toString(alarmStr);
    // Serial.println("Alarm1: ");
    // Serial.print(alarmStr);

    for (int i = 0; i < ALARM_MENU_SIZE; i++) {
      
      AlarmItemInfo alarmItem = alarmItems[i];

      if (currentAlarmItemIndex == i) {
        display.fillRect(alarmItem.x, alarmItem.y, 30, 30, SSD1306_WHITE);
      }

      if (alarmItem.Icon) {
        display.drawBitmap(alarmItem.x, alarmItem.y, alarmItem.Icon, 30, 30, currentAlarmItemIndex == i ? SSD1306_BLACK : SSD1306_WHITE);
      }
      else {

        display.setCursor(alarmItem.x, alarmItem.y);
        display.setTextSize(2);
        display.setTextColor(currentAlarmItemIndex == i ? SSD1306_BLACK : SSD1306_WHITE);

        switch (alarmItem.Code) {
          case AlarmItem::HOUR:
            display.printf("%02d", alarmMinutesTmp / 60);
            break;
          case AlarmItem::MINUTE:
            display.printf("%02d", alarmMinutesTmp % 60);
            break;
          case AlarmItem::ENABLE:
            display.print(alarmEnabledTmp ? "ON" : "OFF");
            break;
        }
      }
    }

    display.display();
  }


  /*encoder handling*/

  encoderCLK = digitalRead(PIN_ENCODER_CLK);

  if (encoderCLK != prevEncoderCLK && encoderCLK == HIGH) {
    Serial.println("Encoder rolled!");

    if (digitalRead(PIN_ENCODER_DT) == HIGH)
      Serial.println("counter-cw");
    else if (digitalRead(PIN_ENCODER_DT) == LOW)
      Serial.println("cw");


    handleEncoder(digitalRead(PIN_ENCODER_DT) != HIGH);
  }

  prevEncoderCLK = encoderCLK; // save last CLK state

  /*buttons handling*/

  modeButton.loop();
  selectButton.loop();

  if (modeButton.isPressed()) {
    Serial.printf("Mode button pressed");
    
    if (isInMenu) {
      
      currentMenuItemIndex = (currentMenuItemIndex + 1) % MENU_SIZE;

      // if (currentMenuItemIndex == MENU_SIZE - 1) { //if last item, go to first
      //   currentMenuItemIndex = 0;
      // }
      // else { //else go to next item
      //   currentMenuItemIndex++; 
      // }

    }
    else if (currentMenuItem.Code == MenuItem::SETTING_ALARM) {
      
      currentAlarmItemIndex = (currentAlarmItemIndex + 1) % ALARM_MENU_SIZE; 
     
    //  if (currentAlarmItemIndex == ALARM_MENU_SIZE - 1) { //if last item, go to first
    //     currentAlarmItemIndex = 0;
    //   }
    //   else { //else go to next item
    //     currentAlarmItemIndex++;
    //   }

      needRedrawAlarmMenu = true;

    }
    else { //not in menu, but also not in alarm setting mode

      isInMenu = true;
      currentMenuItemIndex = 0;
    }

    ///////////////////////////////////////////

    // if (!isInMenu) { //if not in menu, open menu
      
    //   isInMenu = true;
    //   currentMenuItemIndex = 0;
    // }
    // else { //else if in menu
      
    //   if (currentMenuItemIndex == MENU_SIZE - 1) { //if last item, go to first
        
    //     currentMenuItemIndex = 0;
    //   }
    //   else { //else go to next item

    //     currentMenuItemIndex++; 
    //   }
    // }
  }

  if (selectButton.isPressed()) {
    Serial.printf("Select button pressed");

    if (isInMenu) { //if in menu - exit menu - selected menu item displays

      isInMenu = false;

      if (currentMenuItem.Code == MenuItem::SETTING_ALARM) {

        currentAlarmItemIndex = ALARM_MENU_SIZE - 1; //return button is default item in alarm menu
        needRedrawAlarmMenu = true;

      }

    }
    else if (currentMenuItem.Code == MenuItem::SETTING_ALARM) {

      AlarmItemInfo currentAlarmItem = alarmItems[currentAlarmItemIndex];

      if (currentAlarmItem.Code == AlarmItem::RETURN) {
        
        alarmSettings.hour = alarmMinutesTmp / 60;
        alarmSettings.minute = alarmMinutesTmp % 60;
        alarmSettings.enabled = alarmEnabledTmp;

        prefs.begin("settings", false);
        prefs.putBytes("alarm", &alarmSettings, sizeof(alarmSettings));
        prefs.end();
        
        isInMenu = true;
      }
      else if (currentAlarmItem.Code == AlarmItem::ENABLE) {
        
        alarmEnabledTmp = !alarmEnabledTmp;

        needRedrawAlarmMenu = true;
      }
    }
  }
}

void handleEncoder(bool clockwise) {

  MenuItemInfo currentMenuItem = menuItems[currentMenuItemIndex];
  AlarmItemInfo currentAlarmItem = alarmItems[currentAlarmItemIndex];


  // if (currentMenuItem.Code == MenuItem::SETTING_ALARM) {
  //   Serial.println("i'm here");
  //   if (currentAlarmItem.Code == AlarmItem::HOUR) {
  //     Serial.println("and here !");
  //       alarmMinutesTmp = clockwise 
  //         ? (alarmMinutesTmp + 1440 + 60) % 1440
  //         : (alarmMinutesTmp + 1440 - 60) % 1440;

  //     alarmDisplayChanged = true;
  //   }
  //   else if (currentAlarmItem.Code == AlarmItem::MINUTE) {

  //     alarmMinutesTmp = clockwise 
  //       ? (alarmMinutesTmp + 1440 + 5) % 1440
  //       : (alarmMinutesTmp + 1440 - 5) % 1440;

  //     alarmDisplayChanged = true;

  //   }
  // }

  if (currentMenuItem.Code == MenuItem::SETTING_ALARM) {
  
    int minutesDelta = 0;

    if (currentAlarmItem.Code == AlarmItem::HOUR) {
      
      minutesDelta = 60;

    }
    else if (currentAlarmItem.Code == AlarmItem::MINUTE) {

      minutesDelta = 5;

    }

    if (currentAlarmItem.Code == AlarmItem::HOUR || 
        currentAlarmItem.Code == AlarmItem::MINUTE) { 

      if (!clockwise)
        minutesDelta *= -1;

      alarmMinutesTmp = (alarmMinutesTmp + 1440 + minutesDelta) % 1440; 

      needRedrawAlarmMenu = true;
    }
  }
}

void onAlarm() {

}

void tryInitDateTime() {

   //if (true) { //uncomment this, comment next row if need to set time from net each run
  if (rtc.lostPower()) {
    
    Serial.println("RTC lost power, let's set the time!");
    
    bool gotLocalTime = false;
    struct tm timeinfo;

    if (connectToWifi(displayCallback)) {

      Serial.println(gmtOffset_sec);
      Serial.println(daylightOffset_sec);
      Serial.println(ntpServerUrl);
      
      configTime(
        gmtOffset_sec,
        daylightOffset_sec,
        ntpServerUrl
      );

      Serial.println("time configured");

      for (int i = 0; i < 10; i++) {
        
        gotLocalTime = getLocalTime(&timeinfo);
        Serial.println("gotLocalTime:");
        Serial.print(gotLocalTime);
        
        if (gotLocalTime)
          break;

        delay(100);
      }
    }

    rtcAdjustTime(timeinfo, gotLocalTime);
  }  
}

void getDateString(DateTime dateTime, char buffer[], size_t bufferLen) {
  Serial.println(String(daysOfTheWeek[dateTime.dayOfTheWeek()]) + "-" + String(dateTime.year()) + "-" + String(dateTime.month()) + "-" + String(dateTime.day()));
  snprintf(buffer, bufferLen,
    "%s %02d.%02d.%04d",
    daysOfTheWeek[dateTime.dayOfTheWeek()],
    dateTime.day(),
    dateTime.month(),
    dateTime.year());
}

void getTimeString(DateTime dateTime, char buffer[], size_t bufferLen) {
  Serial.println(String(dateTime.hour()) + "-" + String(dateTime.minute()) + "-" + String(dateTime.second()));
  snprintf(buffer, bufferLen,
    "%02d:%02d:%02d",
    dateTime.hour(),
    dateTime.minute(),
    dateTime.second());
}
