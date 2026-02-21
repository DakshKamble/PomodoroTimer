#include <Arduino.h>

#include <FastLED.h>
#include "core/animations.h"
#include "core/timer.h"

#define NUM_LEDS 12 // LED count
#define LED_PIN D7 // LED pin
#define LED_BRIGHTNESS 50
#define ANIMATION_INTERVAL 50

#define COUNTDOWN_TIMER 30000

Timer myTimer; // Creating a Timer object

CRGB leds[NUM_LEDS]; // We use this array to modify leds

void setup() {
  Serial.begin(115200);

  Serial.println("Serial Debugging Started");
  Serial.print("Countdown started Countdown =");
  Serial.println(COUNTDOWN_TIMER);

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS); // Setup LED strip
  FastLED.setBrightness(LED_BRIGHTNESS); // Seting LED brightness

  myTimer.start(COUNTDOWN_TIMER); //TEMP TEST
}

void loop() {
  if(myTimer.isRunning()) { //TEMP TEST
    float fraction = myTimer.getFractionalRemaining();
    anim_renderCountdown(leds, NUM_LEDS, fraction);
    FastLED.show();
  }
}
