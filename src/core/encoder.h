#ifndef ENCODER_H
#define ENCODER_H

#include "config.h"
#include "types.h"

// Encoder direction
enum class EncoderDirection {
    NONE,
    CLOCKWISE,
    COUNTER_CLOCKWISE
};

// Button state
enum class ButtonState {
    RELEASED,
    PRESSED
};

class RotaryEncoder {
public:
    RotaryEncoder();
    
    // Initialization
    void init();
    
    // Update methods (call in main loop)
    void update();
    
    // Encoder reading
    EncoderDirection getDirection();
    bool wasButtonPressed();
    
    // Callback support
    typedef void (*EncoderCallback)(EncoderDirection direction);
    typedef void (*ButtonCallback)();
    
    void setEncoderCallback(EncoderCallback callback);
    void setButtonCallback(ButtonCallback callback);
    
    // Interrupt handler (public so ISR can call it)
    void updateEncoder();

private:
    // Pin states
    bool lastClkState;
    bool lastDtState;
    bool lastSwState;
    
    // Debouncing
    unsigned long lastEncoderTime;
    unsigned long lastButtonTime;
    
    // State tracking
    EncoderDirection lastDirection;
    bool buttonPressed;
    long lastEncoderValue;
    
    // Callbacks
    EncoderCallback encoderCallback;
    ButtonCallback buttonCallback;
    
    // Helper methods
    bool readPin(int pin);
    void handleEncoderChange();
    void handleButtonChange();
};

#endif
