#include <Arduino.h>
#include "animations.h"
#include <math.h>

// Helper Macros
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

// AnimationManager Implementation
AnimationManager::AnimationManager(CRGB* ledArray, int numLeds) 
    : leds(ledArray), numLeds(numLeds), currentAnimation(AnimationType::OFF),
      customAnimationFunc(nullptr), brightness(LED_BRIGHTNESS),
      primaryColor(CRGB::Red), secondaryColor(CRGB::Black) {
}

void AnimationManager::setAnimation(AnimationType type) {
    if (currentAnimation != type) {
        clear(); // Clear LEDs to prevent artifacts from previous animation
        currentAnimation = type;
        customAnimationFunc = nullptr;
    }
}

void AnimationManager::setCustomAnimation(AnimationFunction customFunc) {
    customAnimationFunc = customFunc;
    currentAnimation = AnimationType::OFF; // Use custom function instead
}

void AnimationManager::update(const AnimationParams& params) {
    // Use custom animation if set
    if (customAnimationFunc != nullptr) {
        customAnimationFunc(leds, numLeds, params);
        return;
    }
    
    // Use built-in animations
    switch (currentAnimation) {
        case AnimationType::COUNTDOWN:
            anim_countdown(leds, numLeds, params);
            break;
        case AnimationType::COMET:
            anim_comet(leds, numLeds, params);
            break;
        case AnimationType::PULSE:
            anim_pulse(leds, numLeds, params);
            break;
        case AnimationType::SOLID_COLOR:
            anim_solidColor(leds, numLeds, params);
            break;
        case AnimationType::TIME_SELECTION:
            anim_timeSelection(leds, numLeds, params);
            break;
        case AnimationType::GAUGE_SWEEP:
            anim_gaugeSweep(leds, numLeds, params);
            break;
        case AnimationType::FLASH_COMPLETE:
            anim_flashComplete(leds, numLeds, params);
            break;
        case AnimationType::FLASH_CANCELLED:
            anim_flashCancelled(leds, numLeds, params);
            break;
        case AnimationType::OFF:
        default:
            anim_off(leds, numLeds, params);
            break;
    }
}

void AnimationManager::clear() {
    for (int i = 0; i < numLeds; i++) {
        leds[i] = CRGB::Black;
    }
}

void AnimationManager::show() {
    FastLED.show();
}

void AnimationManager::setBrightness(uint8_t newBrightness) {
    brightness = newBrightness;
    FastLED.setBrightness(brightness);
}

void AnimationManager::setColors(CRGB primary, CRGB secondary) {
    primaryColor = primary;
    secondaryColor = secondary;
}

// ==========================================
// Helper Functions
// ==========================================

uint8_t AnimationManager::applyGamma(uint8_t b) {
    // Simple quadratic gamma approximation (gamma ~ 2.0)
    return (uint8_t)((b * b + 255) >> 8);
}

float AnimationManager::easeOutQuart(float x) {
    // 1 - (1-x)^4
    float inv = 1.0f - x;
    return 1.0f - (inv * inv * inv * inv);
}

float AnimationManager::easeInOutCubic(float x) {
    return x < 0.5f ? 4.0f * x * x * x : 1.0f - pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
}

float AnimationManager::easeOutBounce(float x) {
    const float n1 = 7.5625f;
    const float d1 = 2.75f;
    if (x < 1.0f / d1) {
        return n1 * x * x;
    } else if (x < 2.0f / d1) {
        return n1 * (x -= 1.5f / d1) * x + 0.75f;
    } else if (x < 2.5f / d1) {
        return n1 * (x -= 2.25f / d1) * x + 0.9375f;
    } else {
        return n1 * (x -= 2.625f / d1) * x + 0.984375f;
    }
}

// ==========================================
// Built-in Animation Functions
// ==========================================

void anim_countdown(CRGB* leds, int numLeds, const AnimationParams& params) {
    // Smooth countdown
    float ledsExact = params.progress * numLeds;
    int fullLeds = (int)ledsExact;
    float partialLed = ledsExact - fullLeds;
    
    // Clear
    for (int i = 0; i < numLeds; i++) {
        leds[i] = CRGB::Black;
    }
    
    // Fill full LEDs
    for (int i = 0; i < fullLeds; i++) {
        leds[i] = params.primaryColor;
    }
    
    // Partial LED with gamma correction for smoothness
    if (fullLeds < numLeds && fullLeds >= 0) {
        CRGB color = params.primaryColor;
        // Apply gamma to the partial brightness so it doesn't look too dim too fast
        uint8_t scaledBrightness = AnimationManager::applyGamma((uint8_t)(partialLed * 255));
        color.nscale8_video(scaledBrightness);
        leds[fullLeds] = color;
    }
}

void anim_comet(CRGB* leds, int numLeds, const AnimationParams& params) {
    // Speed: 1 rotation per second (approx)
    float speed = 0.001f; // cycles per ms
    float rawPos = params.timestamp * speed * numLeds;
    
    fadeToBlackBy(leds, numLeds, 20); // Trail fading (adjusted for 60fps)
    
    // Draw comet head and fractional neighbor
    float posInRing = fmod(rawPos, numLeds);
    int headIdx = (int)posInRing;
    float frac = posInRing - headIdx;
    
    // Anti-aliased head
    CRGB colorHead = params.primaryColor;
    CRGB colorNext = params.primaryColor;
    
    // Distribute brightness between head and next pixel
    colorHead.nscale8(255 - (frac * 255));
    colorNext.nscale8(frac * 255);
    
    leds[headIdx] += colorHead;
    leds[(headIdx + 1) % numLeds] += colorNext;
}

void anim_pulse(CRGB* leds, int numLeds, const AnimationParams& params) {
    // Breathing effect using easeInOutCubic
    // 3 second cycle
    float cycle = fmod(params.timestamp, 3000.0f) / 3000.0f; 
    
    // Ping-pong the cycle (0->1->0)
    float pingPong = (cycle < 0.5f) ? (cycle * 2.0f) : ((1.0f - cycle) * 2.0f);
    
    // Apply easing
    float breathe = AnimationManager::easeInOutCubic(pingPong);
    
    // Map 0.0-1.0 to min-max brightness (e.g., 20% to 100%)
    float brightnessFactor = 0.2f + (0.8f * breathe);
    
    CRGB color = params.primaryColor;
    color.nscale8_video(AnimationManager::applyGamma((uint8_t)(brightnessFactor * 255)));
    
    for (int i = 0; i < numLeds; i++) {
        leds[i] = color;
    }
}

void anim_solidColor(CRGB* leds, int numLeds, const AnimationParams& params) {
    for (int i = 0; i < numLeds; i++) {
        leds[i] = params.primaryColor;
    }
}

void anim_timeSelection(CRGB* leds, int numLeds, const AnimationParams& params) {
    // params.progress is 0.0 to 1.0 based on selected time
    
    float ledsExact = params.progress * numLeds;
    int fullLeds = (int)ledsExact;
    float partialLed = ledsExact - fullLeds;
    
    // Breathing effect for the "cursor" (the last active LED)
    // 1.5s breathing cycle
    float breathe = (sin(params.timestamp * 0.004f) + 1.0f) * 0.5f; // 0.0 to 1.0
    
    for (int i = 0; i < numLeds; i++) {
        if (i < fullLeds) {
            // Full on
            leds[i] = CRGB::White;
            
            // If this is the very last fully lit LED and there's no partial, pulse it
            if (i == fullLeds - 1 && partialLed < 0.01f) {
                 // Slight dip in brightness to indicate it's the "active" end
                 uint8_t pulse = 200 + (uint8_t)(55 * breathe); 
                 leds[i].nscale8(pulse);
            }
        } else if (i == fullLeds) {
            // Partial LED (Cursor)
            // Combine partial coverage with breathing
            CRGB color = CRGB::White;
            
            // Base brightness from how "full" this minute step is
            // + Breathing effect superimposed
            float smoothPartial = partialLed; // Could ease this too
            
            // Make the partial LED breathe noticeably to invite interaction
            float pulseFactor = 0.5f + (0.5f * breathe); 
            
            uint8_t finalBrightness = (uint8_t)(smoothPartial * 255 * pulseFactor);
            // Ensure visibility if it's non-zero
            if (finalBrightness < 10 && smoothPartial > 0.01f) finalBrightness = 10;
            
            color.nscale8_video(AnimationManager::applyGamma(finalBrightness));
            leds[i] = color;
        } else {
            leds[i] = CRGB::Black;
        }
    }
}

void anim_gaugeSweep(CRGB* leds, int numLeds, const AnimationParams& params) {
    // Sweep from 'selectedLeds' (params.secondaryColor.r) to 'numLeds'
    int selectedLeds = params.secondaryColor.r;
    if (selectedLeds == 0) selectedLeds = 1;
    
    // Eased progress for mechanical feel
    float easedProgress = AnimationManager::easeOutQuart(params.progress);
    
    // Calculate how many LEDs to add on top of selectedLeds
    int ledsToFill = numLeds - selectedLeds;
    float filledAmount = easedProgress * ledsToFill;
    
    float totalLitExact = selectedLeds + filledAmount;
    int fullLit = (int)totalLitExact;
    float partialLit = totalLitExact - fullLit;
    
    // Clear
    for (int i = 0; i < numLeds; i++) {
        leds[i] = CRGB::Black;
    }
    
    // Render
    for (int i = 0; i < fullLit; i++) {
        leds[i] = params.primaryColor;
        
        // Boost brightness of the leading edge slightly for a "scanner" look?
        // Or just keep it solid red. Solid red is cleaner.
    }
    
    // Partial leading edge
    if (fullLit < numLeds) {
        CRGB color = params.primaryColor;
        // Make the leading edge bright/sharp, with gamma for smoothness
        uint8_t scaledBrightness = AnimationManager::applyGamma((uint8_t)(partialLit * 255));
        color.nscale8_video(scaledBrightness);
        leds[fullLit] = color;
    }
}

void anim_flashComplete(CRGB* leds, int numLeds, const AnimationParams& params) {
    // Heartbeat/Pulse animation for completion
    // Cycle duration: 1000ms
    uint32_t cycleDuration = 1000;
    
    // Calculate cycle phase (0.0 to 1.0)
    float phase = fmod(params.timestamp, cycleDuration) / (float)cycleDuration;
    
    // Ping-pong for pulse (in-out)
    float pingPong = (phase < 0.5f) ? (phase * 2.0f) : ((1.0f - phase) * 2.0f);
    
    // Use easeInOutCubic for natural heartbeat
    float brightness = AnimationManager::easeInOutCubic(pingPong);
    
    CRGB color = params.primaryColor; // Green
    color.nscale8_video(AnimationManager::applyGamma((uint8_t)(brightness * 255)));
    
    for (int i = 0; i < numLeds; i++) {
        leds[i] = color;
    }
}

void anim_flashCancelled(CRGB* leds, int numLeds, const AnimationParams& params) {
    // Similar to Complete but maybe faster/sharper?
    // Let's use the same easeOutBounce for a "error" feel? 
    // Or just a smooth pulse like complete but Red.
    
    // Let's stick to smooth pulse but faster
    uint32_t cycleDuration = 800; // Faster pulse for error/cancel
    
    float phase = fmod(params.timestamp, cycleDuration) / (float)cycleDuration;
    float pingPong = (phase < 0.5f) ? (phase * 2.0f) : ((1.0f - phase) * 2.0f);
    
    float brightness = AnimationManager::easeInOutCubic(pingPong);
    
    CRGB color = params.primaryColor; // Red
    color.nscale8_video(AnimationManager::applyGamma((uint8_t)(brightness * 255)));
    
    for (int i = 0; i < numLeds; i++) {
        leds[i] = color;
    }
}

void anim_off(CRGB* leds, int numLeds, const AnimationParams& params) {
    for (int i = 0; i < numLeds; i++) {
        leds[i] = CRGB::Black;
    }
}