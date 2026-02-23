#include <Arduino.h>
#include <FastLED.h>

// Core system includes
#include "core/config.h"
#include "core/types.h"
#include "core/logger.h"
#include "core/timer.h"
#include "core/animations.h"
#include "core/encoder.h"
#include "core/display.h"

// Global objects
Timer pomodoroTimer;
AnimationManager animManager(nullptr, NUM_LEDS);
RotaryEncoder encoder;
OLEDDisplay oledDisplay;
CRGB leds[NUM_LEDS];

// Application state
AppState currentState = AppState::TIME_SELECTION;
int selectedMinutes = 0;
int encoderStepCount = 0;  // Track encoder steps for sensitivity control
bool systemInitialized = false;
unsigned long flashStartTime = 0;

// Forward declarations
void onTimerComplete();
void onEncoderRotation(EncoderDirection direction);
void onButtonPress();
void onButtonLongPress();
void updateTimeSelection();
void startCountdown();
void updateCountdown();
void updateFlashComplete();
void updateFlashCancelled();
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
        // Update encoder step count
        if (direction == EncoderDirection::CLOCKWISE) {
            encoderStepCount++;
        } else if (direction == EncoderDirection::COUNTER_CLOCKWISE) {
            encoderStepCount--;
        }
        
        // Only update timer when we've accumulated enough steps
        if (abs(encoderStepCount) >= ENCODER_STEPS_PER_INCREMENT) {
            if (encoderStepCount > 0) {
                selectedMinutes = min(selectedMinutes + TIMER_STEP_MINUTES, MAX_TIMER_MINUTES);
            } else {
                selectedMinutes = max(selectedMinutes - TIMER_STEP_MINUTES, 0);
            }
            
            // Reset step count
            encoderStepCount = 0;
            
            LOG_INFOF("Timer set to %d minutes", selectedMinutes);
            updateTimeSelection();
        }
    }
}

void onButtonPress() {
    switch (currentState) {
        case AppState::TIME_SELECTION:
            if (selectedMinutes > 0) {
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
            
        case AppState::TIMER_CANCELLED:
            // Return to time selection
            transitionToState(AppState::TIME_SELECTION);
            break;
    }
}

void onButtonLongPress() {
    switch (currentState) {
        case AppState::COUNTDOWN_RUNNING:
            // Cancel the countdown
            LOG_INFO("Timer cancelled by long press");
            pomodoroTimer.stop();
            transitionToState(AppState::TIMER_CANCELLED);
            break;
            
        default:
            // Long press not handled in other states
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
            selectedMinutes = 0; // Reset to 0
            encoderStepCount = 0; // Reset encoder step count
            updateTimeSelection();
            break;
            
        case AppState::COUNTDOWN_RUNNING:
            animManager.setAnimation(AnimationType::COUNTDOWN);
            animManager.setColors(CRGB::Red, CRGB::Black);
            break;
            
        case AppState::TIMER_COMPLETE:
            animManager.setAnimation(AnimationType::FLASH_COMPLETE);
            flashStartTime = millis();
            oledDisplay.showComplete();
            break;
            
        case AppState::TIMER_CANCELLED:
            animManager.setAnimation(AnimationType::FLASH_CANCELLED);
            flashStartTime = millis();
            oledDisplay.showCancelled();
            break;
    }
}

void updateTimeSelection() {
    // Calculate progress based on selected minutes (0 to 60 minutes)
    float progress = (float)selectedMinutes / MAX_TIMER_MINUTES;
    
    AnimationParams params;
    params.progress = progress;
    params.primaryColor = CRGB::White;
    params.secondaryColor = CRGB::Black;
    params.brightness = LED_BRIGHTNESS;
    params.timestamp = millis();
    
    animManager.update(params);
    animManager.show();
    
    // Update OLED display (convert to seconds for display)
    oledDisplay.showTimeSelection(selectedMinutes * 60);
}

void startCountdown() {
    unsigned long durationMs = selectedMinutes * 60000UL; // Convert minutes to milliseconds
    
    LOG_INFOF("Starting countdown: %d minutes (%lu ms)", selectedMinutes, durationMs);
    
    ErrorCode result = pomodoroTimer.start(durationMs);
    if (result == ErrorCode::SUCCESS) {
        transitionToState(AppState::COUNTDOWN_RUNNING);
    } else {
        LOG_ERROR("Failed to start countdown timer");
    }
}

void updateCountdown() {
    // Calculate the maximum LEDs that should be lit based on selected time
    float maxProgress = (float)selectedMinutes / MAX_TIMER_MINUTES;
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
    
    // Update OLED display
    int remainingSeconds = (int)(pomodoroTimer.getRemaining() / 1000);
    int totalSeconds = selectedMinutes * 60;
    oledDisplay.showCountdown(remainingSeconds, totalSeconds);
}

void updateFlashComplete() {
    AnimationParams params;
    params.progress = 0.0f; // Not used in flash animation
    params.primaryColor = CRGB::Green;
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

void updateFlashCancelled() {
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
    
    // Initialize OLED display
    oledDisplay.init();
    
    // Initialize rotary encoder
    encoder.init();
    encoder.setEncoderCallback(onEncoderRotation);
    encoder.setButtonCallback(onButtonPress);
    encoder.setButtonLongPressCallback(onButtonLongPress);
    
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
            
        case AppState::TIMER_CANCELLED:
            updateFlashCancelled();
            break;
    }
    
    // Small delay to prevent overwhelming the system
    delay(ANIMATION_INTERVAL);
}
