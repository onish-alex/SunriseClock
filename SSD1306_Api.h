#pragma once

//#include "ESP32S_Pins.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

extern Adafruit_SSD1306 display;

struct DisplayOptions {
  int size = 1;
  int color = SSD1306_WHITE;
  short* cursorX = nullptr;
  short* cursorY = nullptr;
};

void setupOledScreen();

void displayClearResetPosition();

void displayPrintln(const char* text, DisplayOptions options = {});

void displayPrint(const char* text, DisplayOptions options = {});

void displayCallback(const char* text, bool clearScreen, int size = 1);
