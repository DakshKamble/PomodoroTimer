#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <FastLED.h>

void anim_comet(CRGB *leds, int NUM_LEDS);
void anim_countDown(CRGB *leds, int NUM_LEDS, float fraction);

#endif