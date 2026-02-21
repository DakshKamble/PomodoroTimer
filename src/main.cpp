#include <Arduino.h>

#include <FastLED.h>

#define NUM_LEDS 12 // LED Count
#define LED_PIN D7 // LED Pin
#define LED_BRIGHTNESS 50
#define ANIMATION_INTERVAL 50

unsigned long lastUpdate = 0;
CRGB leds[NUM_LEDS]; // We use this array to modify leds

void fill_led();

void setup() {

  Serial.begin(115200);
  Serial.println("Serial Debugging Started");

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS); // Setup LED Strip
  FastLED.setBrightness(LED_BRIGHTNESS);

}

void loop() {

  unsigned long now = millis();

  if(now - lastUpdate >= ANIMATION_INTERVAL) {
    lastUpdate = now;
    static int position = 0;

    // Fade existing LEDs (creates trail)
    fadeToBlackBy(leds, NUM_LEDS, 50);

    // Light current position
    leds[position] = CRGB::Red;

    // Show update
    FastLED.show();

    // Move forward
    position++;
    if(position >= NUM_LEDS) {
      position = 0;
    }
  }
}