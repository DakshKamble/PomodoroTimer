#include <Arduino.h>
#include "encoder.h"
#include "logger.h"

// Static variables for interrupt handling
static volatile int lastEncoded = 0;
static volatile long encoderValue = 0;
static RotaryEncoder* encoderInstance = nullptr;

// Interrupt service routine
void IRAM_ATTR encoderISR() {
    if (encoderInstance) {
        encoderInstance->updateEncoder();
    }
}

RotaryEncoder::RotaryEncoder() 
    : lastClkState(false), lastDtState(false), lastSwState(true),
      lastEncoderTime(0), lastButtonTime(0),
      lastDirection(EncoderDirection::NONE), buttonPressed(false),
      buttonLongPressed(false), lastEncoderValue(0), buttonPressStartTime(0),
      encoderCallback(nullptr), buttonCallback(nullptr), buttonLongPressCallback(nullptr) {
    encoderInstance = this;
}

void RotaryEncoder::init() {
    // Configure encoder pins as inputs with pullups
    pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
    pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
    pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
    
    // Read initial states
    lastClkState = readPin(ENCODER_CLK_PIN);
    lastDtState = readPin(ENCODER_DT_PIN);
    lastSwState = readPin(ENCODER_SW_PIN);
    
    // Setup interrupts for encoder pins
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoderISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_DT_PIN), encoderISR, CHANGE);
    
    // Initialize encoder state
    encoderValue = 0;
    lastEncoderValue = 0;
    lastEncoded = 0;
    
    LOG_INFO("Rotary encoder initialized with interrupts");
}

void RotaryEncoder::update() {
    handleEncoderChange();
    handleButtonChange();
}

EncoderDirection RotaryEncoder::getDirection() {
    EncoderDirection dir = lastDirection;
    lastDirection = EncoderDirection::NONE; // Reset after reading
    return dir;
}

bool RotaryEncoder::wasButtonPressed() {
    bool pressed = buttonPressed;
    buttonPressed = false; // Reset after reading
    return pressed;
}

bool RotaryEncoder::wasButtonLongPressed() {
    bool longPressed = buttonLongPressed;
    buttonLongPressed = false; // Reset after reading
    return longPressed;
}

void RotaryEncoder::setEncoderCallback(EncoderCallback callback) {
    encoderCallback = callback;
}

void RotaryEncoder::setButtonCallback(ButtonCallback callback) {
    buttonCallback = callback;
}

void RotaryEncoder::setButtonLongPressCallback(ButtonLongPressCallback callback) {
    buttonLongPressCallback = callback;
}

bool RotaryEncoder::readPin(int pin) {
    return digitalRead(pin) == HIGH;
}

void RotaryEncoder::handleEncoderChange() {
    // Check if encoder value has changed
    if (encoderValue != lastEncoderValue) {
        EncoderDirection direction;
        
        if (encoderValue > lastEncoderValue) {
            direction = EncoderDirection::CLOCKWISE;
        } else {
            direction = EncoderDirection::COUNTER_CLOCKWISE;
        }
        
        lastDirection = direction;
        lastEncoderValue = encoderValue;
        
        LOG_DEBUGF("Encoder: %s (Value: %ld)", 
                  (direction == EncoderDirection::CLOCKWISE) ? "CW" : "CCW",
                  encoderValue);
        
        // Call callback if set
        if (encoderCallback != nullptr) {
            encoderCallback(direction);
        }
    }
}

void RotaryEncoder::updateEncoder() {
    int MSB = digitalRead(ENCODER_CLK_PIN); // MSB = most significant bit
    int LSB = digitalRead(ENCODER_DT_PIN);  // LSB = least significant bit
    
    int encoded = (MSB << 1) | LSB; // converting the 2 pin value to single number
    int sum = (lastEncoded << 2) | encoded; // adding it to the previous encoded value
    
    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        encoderValue++;
    }
    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        encoderValue--;
    }
    
    lastEncoded = encoded; // store this value for next time
}

void RotaryEncoder::handleButtonChange() {
    unsigned long currentTime = millis();
    bool currentSwState = readPin(ENCODER_SW_PIN);
    
    // Button state changed
    if (currentSwState != lastSwState) {
        if (currentTime - lastButtonTime < ENCODER_DEBOUNCE_MS) {
            return; // Debounce
        }
        
        if (currentSwState == false) {
            // Button pressed (active LOW)
            buttonPressStartTime = currentTime;
            LOG_DEBUG("Button press started");
        } else {
            // Button released
            unsigned long pressDuration = currentTime - buttonPressStartTime;
            
            if (pressDuration >= ENCODER_LONG_PRESS_MS) {
                // Long press detected
                buttonLongPressed = true;
                LOG_DEBUG("Button long pressed");
                
                if (buttonLongPressCallback != nullptr) {
                    buttonLongPressCallback();
                }
            } else if (pressDuration > ENCODER_DEBOUNCE_MS) {
                // Short press detected
                buttonPressed = true;
                LOG_DEBUG("Button short pressed");
                
                if (buttonCallback != nullptr) {
                    buttonCallback();
                }
            }
        }
        
        lastButtonTime = currentTime;
        lastSwState = currentSwState;
    }
}
