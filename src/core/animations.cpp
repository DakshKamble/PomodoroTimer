#include <Arduino.h>
#include "core/animations.h"

void anim_comet(CRGB* leds, int numLeds) {

    static int position = 0;

    fadeToBlackBy(leds, numLeds, 50);

    leds[position] = CRGB::Red;

    position++;
    if (position >= numLeds) {
        position = 0;
    }
}

void anim_countDown(CRGB *leds, int NUM_LEDS, float fraction) {
    
}