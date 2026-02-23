#ifndef TIMER_H
#define TIMER_H

#include "types.h"

// Forward declaration for callback function
typedef void (*TimerCallback)(void);

class Timer {
public:
    Timer();
    
    // Core timer operations
    ErrorCode start(unsigned long durationMs);
    ErrorCode stop();
    ErrorCode pause();
    ErrorCode resume();
    ErrorCode reset();
    
    // State queries
    TimerState getState() const;
    bool isRunning() const;
    bool isPaused() const;
    bool isCompleted() const;
    
    // Time queries
    unsigned long getRemaining() const;
    unsigned long getElapsed() const;
    unsigned long getDuration() const;
    float getFractionalRemaining() const;
    float getFractionalElapsed() const;
    
    // Callback management
    void setOnCompleteCallback(TimerCallback callback);
    void setOnTickCallback(TimerCallback callback);
    
    // Update method (call in main loop)
    void update();
    
private:
    unsigned long startTime;
    unsigned long pausedTime;
    unsigned long duration;
    TimerState state;
    
    TimerCallback onCompleteCallback;
    TimerCallback onTickCallback;
    
    // Helper methods
    unsigned long getCurrentTime() const;
    void handleTimerCompletion();
};

#endif