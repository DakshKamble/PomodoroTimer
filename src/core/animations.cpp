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

void anim_renderCountdown(CRGB *leds, int NUM_LEDS, float fraction) {
    
    float ledsExact = fraction * NUM_LEDS;

    int fullLeds = (int)ledsExact;
    float partialLeds = ledsExact - fullLeds;

    for(int i = 0; i < NUM_LEDS; i++) {
        if(i < fullLeds) {
            leds[i] = CRGB::Red;
        }
        
        else if(i == fullLeds) {
            uint8_t brightness = partialLeds * 255;
            leds[i] = CRGB(brightness, 0, 0);
        }

        else {
            leds[i] = CRGB::Black;
        }
    }

}