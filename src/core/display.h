#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>
#include <Wire.h>
#include "config.h"
#include "types.h"

class OLEDDisplay {
public:
    OLEDDisplay();
    
    // Initialization
    void init();
    
    // Display methods
    void showTimeSelection(int seconds);
    void showCountdown(int remainingSeconds, int totalSeconds);
    void showComplete();
    void showCancelled();
    void clear();
    
    // Update display
    void update();

private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C display;
    
    // Helper methods
    void drawCenteredText(const char* text, int y);
    void drawProgressBar(int current, int total, int x, int y, int width, int height);
    String formatTime(int seconds);
};

#endif
