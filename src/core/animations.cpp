#include <Arduino.h>
#include "animations.h"

// AnimationManager Implementation
AnimationManager::AnimationManager(CRGB* ledArray, int numLeds) 
    : leds(ledArray), numLeds(numLeds), currentAnimation(AnimationType::OFF),
      customAnimationFunc(nullptr), brightness(LED_BRIGHTNESS),
      primaryColor(CRGB::Red), secondaryColor(CRGB::Black) {
}

void AnimationManager::setAnimation(AnimationType type) {
    currentAnimation = type;
    customAnimationFunc = nullptr;
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

// Built-in Animation Functions
void anim_countdown(CRGB* leds, int numLeds, const AnimationParams& params) {
    // Calculate how many LEDs should be lit based on remaining time
    float ledsExact = params.progress * numLeds;
    int fullLeds = (int)ledsExact;
    float partialLed = ledsExact - fullLeds;
    
    // Turn off all LEDs first
    for (int i = 0; i < numLeds; i++) {
        leds[i] = CRGB::Black;
    }
    
    // Light up LEDs from 0 to the remaining count (countdown from selected position)
    for (int i = 0; i < fullLeds; i++) {
        leds[i] = params.primaryColor;
    }
    
    // Handle partial LED for smooth transition
    if (fullLeds < numLeds && partialLed > 0) {
        CRGB color = params.primaryColor;
        color.nscale8_video((uint8_t)(partialLed * 255));
        leds[fullLeds] = color;
    }
}

void anim_comet(CRGB* leds, int numLeds, const AnimationParams& params) {
    static uint32_t lastUpdate = 0;
    static int position = 0;
    
    // Update position based on time
    if (params.timestamp - lastUpdate > 100) { // Update every 100ms
        position = (position + 1) % numLeds;
        lastUpdate = params.timestamp;
    }
    
    // Fade all LEDs
    fadeToBlackBy(leds, numLeds, 50);
    
    // Set comet head
    leds[position] = params.primaryColor;
    
    // Add tail effect
    for (int i = 1; i <= 3 && i <= numLeds; i++) {
        int tailPos = (position - i + numLeds) % numLeds;
        CRGB tailColor = params.primaryColor;
        tailColor.nscale8_video(255 / (i + 1));
        leds[tailPos] = tailColor;
    }
}

void anim_pulse(CRGB* leds, int numLeds, const AnimationParams& params) {
    // Create breathing effect based on timestamp
    float breathe = (sin(params.timestamp * 0.005f) + 1.0f) * 0.5f; // 0.0 to 1.0
    
    CRGB color = params.primaryColor;
    color.nscale8_video((uint8_t)(breathe * 255));
    
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
    // Show white LEDs based on progress (0.0 = no LEDs, 1.0 = all LEDs)
    float ledsExact = params.progress * numLeds;
    int fullLeds = (int)ledsExact;
    float partialLed = ledsExact - fullLeds;
    
    for (int i = 0; i < numLeds; i++) {
        if (i < fullLeds) {
            leds[i] = CRGB::White;
        } else if (i == fullLeds && partialLed > 0) {
            // Smooth transition for partial LED
            CRGB color = CRGB::White;
            color.nscale8_video((uint8_t)(partialLed * 255));
            leds[i] = color;
        } else {
            leds[i] = CRGB::Black;
        }
    }
}

void anim_flashComplete(CRGB* leds, int numLeds, const AnimationParams& params) {
    // Create smooth fade animation for completion notification
    static uint32_t flashStartTime = 0;
    static int currentCycle = 0;
    
    if (flashStartTime == 0) {
        flashStartTime = params.timestamp;
        currentCycle = 0;
    }
    
    uint32_t elapsed = params.timestamp - flashStartTime;
    uint32_t cycleDuration = 1000; // 1 second per complete fade cycle (in + out)
    uint32_t totalDuration = FLASH_ANIMATION_CYCLES * cycleDuration;
    
    if (elapsed >= totalDuration) {
        // Animation complete, turn off all LEDs
        for (int i = 0; i < numLeds; i++) {
            leds[i] = CRGB::Black;
        }
        flashStartTime = 0; // Reset for next time
        return;
    }
    
    // Calculate smooth fade brightness
    uint32_t cycleTime = elapsed % cycleDuration;
    float cycleProgress = (float)cycleTime / cycleDuration;
    
    float brightness;
    if (cycleProgress < 0.5f) {
        // Fade in (0 to 1)
        brightness = cycleProgress * 2.0f;
    } else {
        // Fade out (1 to 0)
        brightness = (1.0f - cycleProgress) * 2.0f;
    }
    
    // Apply smooth brightness to all LEDs
    CRGB color = CRGB::Green;
    color.nscale8_video((uint8_t)(brightness * 255));
    
    for (int i = 0; i < numLeds; i++) {
        leds[i] = color;
    }
}

void anim_gaugeSweep(CRGB* leds, int numLeds, const AnimationParams& params) {
    // Gauge-style sweep animation from selected position to full ring
    // params.progress represents the sweep progress (0.0 to 1.0)
    // params.secondaryColor.r stores the selected LED count (hack to pass extra data)
    
    int selectedLeds = params.secondaryColor.r; // Number of LEDs for selected time
    if (selectedLeds == 0) selectedLeds = 1; // Minimum 1 LED
    
    // Calculate current sweep position
    int totalSweepLeds = numLeds - selectedLeds; // LEDs to sweep
    int currentSweepLeds = (int)(params.progress * totalSweepLeds);
    int currentTotalLeds = selectedLeds + currentSweepLeds;
    
    // Clear all LEDs first
    for (int i = 0; i < numLeds; i++) {
        leds[i] = CRGB::Black;
    }
    
    // Turn ALL selected LEDs red first (not white)
    for (int i = 0; i < selectedLeds; i++) {
        leds[i] = params.primaryColor; // Red
        leds[i].nscale8(params.brightness);
    }
    
    // Show swept LEDs in red (continuing the sweep)
    for (int i = selectedLeds; i < currentTotalLeds; i++) {
        leds[i] = params.primaryColor; // Red
        leds[i].nscale8(params.brightness);
    }
}

void anim_flashCancelled(CRGB* leds, int numLeds, const AnimationParams& params) {
    // Create smooth red fade animation for cancel notification
    static uint32_t flashStartTime = 0;
    static int currentCycle = 0;
    
    if (flashStartTime == 0) {
        flashStartTime = params.timestamp;
        currentCycle = 0;
    }
    
    uint32_t elapsed = params.timestamp - flashStartTime;
    uint32_t cycleDuration = 1000; // 1 second per complete fade cycle (in + out)
    uint32_t totalDuration = FLASH_ANIMATION_CYCLES * cycleDuration;
    
    if (elapsed >= totalDuration) {
        // Animation complete, turn off all LEDs
        for (int i = 0; i < numLeds; i++) {
            leds[i] = CRGB::Black;
        }
        flashStartTime = 0; // Reset for next time
        return;
    }
    
    // Calculate smooth fade brightness
    uint32_t cycleTime = elapsed % cycleDuration;
    float cycleProgress = (float)cycleTime / cycleDuration;
    
    float brightness;
    if (cycleProgress < 0.5f) {
        // Fade in (0 to 1)
        brightness = cycleProgress * 2.0f;
    } else {
        // Fade out (1 to 0)
        brightness = (1.0f - cycleProgress) * 2.0f;
    }
    
    // Apply smooth brightness to all LEDs with RED color for cancel
    CRGB color = CRGB::Red;
    color.nscale8_video((uint8_t)(brightness * 255));
    
    for (int i = 0; i < numLeds; i++) {
        leds[i] = color;
    }
}

void anim_off(CRGB* leds, int numLeds, const AnimationParams& params) {
    for (int i = 0; i < numLeds; i++) {
        leds[i] = CRGB::Black;
    }
}