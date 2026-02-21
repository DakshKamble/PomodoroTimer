#include <Arduino.h>

#include <FastLED.h>

#define NUM_LEDS 12
#define LED_PIN D7

CRGB leds[NUM_LEDS];

void setup() {

  Serial.begin(115200);
  Serial.println("Serial Debugging Started");

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

}

void loop() {
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(40);

}

