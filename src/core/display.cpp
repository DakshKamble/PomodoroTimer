#include <Arduino.h>
#include "display.h"
#include "logger.h"

OLEDDisplay::OLEDDisplay() : display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE) {
}

void OLEDDisplay::init() {
    // Initialize I2C with custom pins
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    
    // Initialize display
    display.begin();
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    
    // Show startup message
    display.drawStr(0, 15, "Pomodoro Timer");
    display.drawStr(0, 30, "Ready...");
    display.sendBuffer();
    
    LOG_INFO("OLED display initialized");
}

void OLEDDisplay::showTimeSelection(int seconds) {
    display.clearBuffer();
    
    // Title
    display.setFont(u8g2_font_ncenB08_tr);
    drawCenteredText("SET TIMER", 15);
    
    // Time display
    display.setFont(u8g2_font_ncenB18_tr);
    String timeStr = formatTime(seconds);
    drawCenteredText(timeStr.c_str(), 40);
    
    // Instructions
    display.setFont(u8g2_font_6x10_tr);
    drawCenteredText("Rotate to adjust", 55);
    drawCenteredText("Press to start", 64);
    
    display.sendBuffer();
}

void OLEDDisplay::showCountdown(int remainingSeconds, int totalSeconds) {
    display.clearBuffer();
    
    // Title
    display.setFont(u8g2_font_ncenB08_tr);
    drawCenteredText("COUNTDOWN", 15);
    
    // Time display
    display.setFont(u8g2_font_ncenB18_tr);
    String timeStr = formatTime(remainingSeconds);
    drawCenteredText(timeStr.c_str(), 35);
    
    // Progress bar
    drawProgressBar(totalSeconds - remainingSeconds, totalSeconds, 10, 45, 108, 8);
    
    // Instructions
    display.setFont(u8g2_font_6x10_tr);
    drawCenteredText("Hold 3s to cancel", 64);
    
    display.sendBuffer();
}

void OLEDDisplay::showComplete() {
    display.clearBuffer();
    
    // Title
    display.setFont(u8g2_font_ncenB12_tr);
    drawCenteredText("COMPLETE!", 25);
    
    // Message
    display.setFont(u8g2_font_ncenB08_tr);
    drawCenteredText("Timer finished", 40);
    
    // Instructions
    display.setFont(u8g2_font_6x10_tr);
    drawCenteredText("Press any key", 55);
    drawCenteredText("to continue", 64);
    
    display.sendBuffer();
}

void OLEDDisplay::showCancelled() {
    display.clearBuffer();
    
    // Title
    display.setFont(u8g2_font_ncenB12_tr);
    drawCenteredText("CANCELLED", 25);
    
    // Message
    display.setFont(u8g2_font_ncenB08_tr);
    drawCenteredText("Timer stopped", 40);
    
    // Instructions
    display.setFont(u8g2_font_6x10_tr);
    drawCenteredText("Press any key", 55);
    drawCenteredText("to continue", 64);
    
    display.sendBuffer();
}

void OLEDDisplay::clear() {
    display.clearBuffer();
    display.sendBuffer();
}

void OLEDDisplay::update() {
    // This method can be used for any periodic display updates if needed
}

void OLEDDisplay::drawCenteredText(const char* text, int y) {
    int textWidth = display.getStrWidth(text);
    int x = (OLED_WIDTH - textWidth) / 2;
    display.drawStr(x, y, text);
}

void OLEDDisplay::drawProgressBar(int current, int total, int x, int y, int width, int height) {
    // Draw border
    display.drawFrame(x, y, width, height);
    
    // Draw filled portion
    if (total > 0) {
        int fillWidth = (current * (width - 2)) / total;
        if (fillWidth > 0) {
            display.drawBox(x + 1, y + 1, fillWidth, height - 2);
        }
    }
}

String OLEDDisplay::formatTime(int seconds) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    
    String result = "";
    if (minutes > 0) {
        result += String(minutes) + "m ";
    }
    result += String(secs) + "s";
    
    return result;
}
