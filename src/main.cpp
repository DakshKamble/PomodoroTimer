#include <Arduino.h>
#include <FastLED.h>

// Core system includes
#include "core/config.h"
#include "core/types.h"
#include "core/logger.h"
#include "core/timer.h"
#include "core/animations.h"
#include "core/encoder.h"

// Global objects
Timer pomodoroTimer;
AnimationManager animManager(nullptr, NUM_LEDS);
RotaryEncoder encoder;
CRGB leds[NUM_LEDS];

// Application state
AppState currentState = AppState::TIME_SELECTION;
int selectedSeconds = 0;
bool systemInitialized = false;
unsigned long flashStartTime = 0;

// Forward declarations
void onTimerComplete();
void onEncoderRotation(EncoderDirection direction);
void onButtonPress();
void updateTimeSelection();
void startCountdown();
void updateCountdown();
void updateFlashComplete();
void transitionToState(AppState newState);

// Timer callback functions
void onTimerComplete() {
    LOG_INFO("Timer completed!");
    transitionToState(AppState::TIMER_COMPLETE);
}

void onTimerTick() {
    // Called every timer update - can be used for periodic tasks
}

// Encoder callback functions
void onEncoderRotation(EncoderDirection direction) {
    if (currentState == AppState::TIME_SELECTION) {
        if (direction == EncoderDirection::CLOCKWISE) {
            selectedSeconds = min(selectedSeconds + TIMER_STEP_SECONDS, MAX_TIMER_SECONDS);
        } else if (direction == EncoderDirection::COUNTER_CLOCKWISE) {
            selectedSeconds = max(selectedSeconds - TIMER_STEP_SECONDS, 0);
        }
        
        LOG_INFOF("Timer set to %d seconds", selectedSeconds);
        updateTimeSelection();
    }
}

void onButtonPress() {
    switch (currentState) {
        case AppState::TIME_SELECTION:
            if (selectedSeconds > 0) {
                startCountdown();
            }
            break;
            
        case AppState::COUNTDOWN_RUNNING:
            // Could add pause functionality here if needed
            break;
            
        case AppState::TIMER_COMPLETE:
            // Return to time selection
            transitionToState(AppState::TIME_SELECTION);
            break;
    }
}

// State management functions
void transitionToState(AppState newState) {
    LOG_INFOF("State transition: %d -> %d", (int)currentState, (int)newState);
    currentState = newState;
    
    switch (newState) {
        case AppState::TIME_SELECTION:
            animManager.setAnimation(AnimationType::TIME_SELECTION);
            selectedSeconds = 0; // Reset to 0
            updateTimeSelection();
            break;
            
        case AppState::COUNTDOWN_RUNNING:
            animManager.setAnimation(AnimationType::COUNTDOWN);
            animManager.setColors(CRGB::Red, CRGB::Black);
            break;
            
        case AppState::TIMER_COMPLETE:
            animManager.setAnimation(AnimationType::FLASH_COMPLETE);
            flashStartTime = millis();
            break;
    }
}

void updateTimeSelection() {
    // Calculate progress based on selected seconds (0 to 60 seconds)
    float progress = (float)selectedSeconds / MAX_TIMER_SECONDS;
    
    AnimationParams params;
    params.progress = progress;
    params.primaryColor = CRGB::White;
    params.secondaryColor = CRGB::Black;
    params.brightness = LED_BRIGHTNESS;
    params.timestamp = millis();
    
    animManager.update(params);
    animManager.show();
}

void startCountdown() {
    unsigned long durationMs = selectedSeconds * 1000UL; // Convert seconds to milliseconds
    
    LOG_INFOF("Starting countdown: %d seconds (%lu ms)", selectedSeconds, durationMs);
    
    ErrorCode result = pomodoroTimer.start(durationMs);
    if (result == ErrorCode::SUCCESS) {
        transitionToState(AppState::COUNTDOWN_RUNNING);
    } else {
        LOG_ERROR("Failed to start countdown timer");
    }
}

void updateCountdown() {
    // Calculate the maximum LEDs that should be lit based on selected time
    float maxProgress = (float)selectedSeconds / MAX_TIMER_SECONDS;
    float currentProgress = pomodoroTimer.getFractionalRemaining();
    
    // Scale the progress to only use the selected portion of the ring
    float scaledProgress = currentProgress * maxProgress;
    
    AnimationParams params;
    params.progress = scaledProgress;
    params.primaryColor = CRGB::Red;
    params.secondaryColor = CRGB::Black;
    params.brightness = LED_BRIGHTNESS;
    params.timestamp = millis();
    
    animManager.update(params);
    animManager.show();
}

void updateFlashComplete() {
    AnimationParams params;
    params.progress = 0.0f; // Not used in flash animation
    params.primaryColor = CRGB::Red;
    params.secondaryColor = CRGB::Black;
    params.brightness = LED_BRIGHTNESS;
    params.timestamp = millis();
    
    animManager.update(params);
    animManager.show();
    
    // Check if flash animation should end (3 seconds total)
    if (millis() - flashStartTime > (FLASH_ANIMATION_CYCLES * 1000UL)) {
        transitionToState(AppState::TIME_SELECTION);
    }
}

// System initialization
bool initializeSystem() {
    LOG_INFO("Initializing Pomodoro Timer System...");
    
    // Initialize FastLED
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
    
    // Initialize animation manager with LED array
    animManager = AnimationManager(leds, NUM_LEDS);
    animManager.setAnimation(AnimationType::TIME_SELECTION);
    
    // Initialize rotary encoder
    encoder.init();
    encoder.setEncoderCallback(onEncoderRotation);
    encoder.setButtonCallback(onButtonPress);
    
    // Setup timer callbacks
    pomodoroTimer.setOnCompleteCallback(onTimerComplete);
    pomodoroTimer.setOnTickCallback(onTimerTick);
    
    LOG_INFO("System initialization complete");
    return true;
}

void setup() {
    // Initialize logging first
    Logger::init();
    LOG_INFO("=== Pomodoro Timer with Rotary Encoder ===");
    
    // Initialize system components
    systemInitialized = initializeSystem();
    
    if (!systemInitialized) {
        LOG_ERROR("System initialization failed!");
        return;
    }
    
    // Start in time selection mode
    transitionToState(AppState::TIME_SELECTION);
    
    LOG_INFO("System ready. Rotate encoder to set timer (0-60s), press to start.");
}

void loop() {
    if (!systemInitialized) {
        return;
    }
    
    // Update input devices
    encoder.update();
    pomodoroTimer.update();
    
    // Handle state-specific updates
    switch (currentState) {
        case AppState::TIME_SELECTION:
            // Time selection display is updated in encoder callback
            break;
            
        case AppState::COUNTDOWN_RUNNING:
            updateCountdown();
            break;
            
        case AppState::TIMER_COMPLETE:
            updateFlashComplete();
            break;
    }
    
    // Small delay to prevent overwhelming the system
    delay(ANIMATION_INTERVAL);
}
