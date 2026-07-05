#include "SSD1306_Api.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setupOledScreen() {
  // Wait for display
  delay(500);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000);  // Pause for 2 seconds

  display.clearDisplay();

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  //display.display();
  //delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  /*
   // Invert and restore display, pausing in-between
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);
  */
}

void handleDisplayOptions(DisplayOptions options) {
  
  if (options.cursorX && options.cursorY)
    display.setCursor(*options.cursorX, *options.cursorY);

  display.setTextSize(options.size);
  display.setTextColor(options.color);
}

void displayPrintln(const char* text, DisplayOptions options) {
  
  handleDisplayOptions(options);

  display.println(text);
}

void displayPrint(const char* text, DisplayOptions options) {
  
  handleDisplayOptions(options);
  
  display.print(text);
}

void displayClearResetPosition() {
    display.clearDisplay();
    display.setCursor(0, 0);
}

void displayCallback(const char* text, bool clearScreen, int size) {
  
  if (clearScreen) {
    display.clearDisplay();
    displayPrintln(text, DisplayOptions{2});
  }
  else {
    
    displayPrint(text, DisplayOptions{2});
  }

  display.display();
}
