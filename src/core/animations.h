#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <FastLED.h>
#include "types.h"
#include "config.h"

// Animation parameters structure
struct AnimationParams {
    float progress;        // 0.0 to 1.0
    CRGB primaryColor;
    CRGB secondaryColor;
    uint8_t brightness;
    uint32_t timestamp;    // For time-based animations
};

// Animation function pointer type
typedef void (*AnimationFunction)(CRGB* leds, int numLeds, const AnimationParams& params);

// Animation Manager Class
class AnimationManager {
public:
    AnimationManager(CRGB* ledArray, int numLeds);
    
    // Animation control
    void setAnimation(AnimationType type);
    void setCustomAnimation(AnimationFunction customFunc);
    void update(const AnimationParams& params);
    void clear();
    void show();
    
    // Color management
    void setBrightness(uint8_t brightness);
    void setColors(CRGB primary, CRGB secondary = CRGB::Black);
    
private:
    CRGB* leds;
    int numLeds;
    AnimationType currentAnimation;
    AnimationFunction customAnimationFunc;
    uint8_t brightness;
    CRGB primaryColor;
    CRGB secondaryColor;
};

// Built-in animation functions
void anim_countdown(CRGB* leds, int numLeds, const AnimationParams& params);
void anim_comet(CRGB* leds, int numLeds, const AnimationParams& params);
void anim_pulse(CRGB* leds, int numLeds, const AnimationParams& params);
void anim_solidColor(CRGB* leds, int numLeds, const AnimationParams& params);
void anim_timeSelection(CRGB* leds, int numLeds, const AnimationParams& params);
void anim_flashComplete(CRGB* leds, int numLeds, const AnimationParams& params);
void anim_off(CRGB* leds, int numLeds, const AnimationParams& params);

#endif