#include <Arduino.h>
#include "timer.h"

Timer::Timer() : startTime(0), pausedTime(0), duration(0), 
                 state(TimerState::STOPPED), onCompleteCallback(nullptr), 
                 onTickCallback(nullptr) {
}

ErrorCode Timer::start(unsigned long durationMs) {
    if (durationMs == 0) {
        return ErrorCode::INVALID_DURATION;
    }
    
    duration = durationMs;
    startTime = getCurrentTime();
    pausedTime = 0;
    state = TimerState::RUNNING;
    
    return ErrorCode::SUCCESS;
}

ErrorCode Timer::stop() {
    state = TimerState::STOPPED;
    startTime = 0;
    pausedTime = 0;
    return ErrorCode::SUCCESS;
}

ErrorCode Timer::pause() {
    if (state != TimerState::RUNNING) {
        return ErrorCode::TIMER_NOT_RUNNING;
    }
    
    pausedTime = getCurrentTime();
    state = TimerState::PAUSED;
    return ErrorCode::SUCCESS;
}

ErrorCode Timer::resume() {
    if (state != TimerState::PAUSED) {
        return ErrorCode::TIMER_NOT_RUNNING;
    }
    
    // Adjust start time to account for paused duration
    unsigned long pausedDuration = getCurrentTime() - pausedTime;
    startTime += pausedDuration;
    state = TimerState::RUNNING;
    
    return ErrorCode::SUCCESS;
}

ErrorCode Timer::reset() {
    state = TimerState::STOPPED;
    startTime = 0;
    pausedTime = 0;
    return ErrorCode::SUCCESS;
}

TimerState Timer::getState() const {
    return state;
}

bool Timer::isRunning() const {
    return state == TimerState::RUNNING;
}

bool Timer::isPaused() const {
    return state == TimerState::PAUSED;
}

bool Timer::isCompleted() const {
    return state == TimerState::COMPLETED;
}

unsigned long Timer::getRemaining() const {
    if (state != TimerState::RUNNING) {
        return (state == TimerState::COMPLETED) ? 0 : duration;
    }
    
    unsigned long elapsed = getCurrentTime() - startTime;
    
    if (elapsed >= duration) {
        return 0;
    }
    
    return duration - elapsed;
}

unsigned long Timer::getElapsed() const {
    if (state == TimerState::STOPPED) {
        return 0;
    }
    
    if (state == TimerState::PAUSED) {
        return pausedTime - startTime;
    }
    
    unsigned long elapsed = getCurrentTime() - startTime;
    return (elapsed > duration) ? duration : elapsed;
}

unsigned long Timer::getDuration() const {
    return duration;
}

float Timer::getFractionalRemaining() const {
    if (duration == 0) {
        return 0.0f;
    }
    
    return (float)getRemaining() / duration;
}

float Timer::getFractionalElapsed() const {
    if (duration == 0) {
        return 0.0f;
    }
    
    return (float)getElapsed() / duration;
}

void Timer::setOnCompleteCallback(TimerCallback callback) {
    onCompleteCallback = callback;
}

void Timer::setOnTickCallback(TimerCallback callback) {
    onTickCallback = callback;
}

void Timer::update() {
    if (state == TimerState::RUNNING) {
        unsigned long elapsed = getCurrentTime() - startTime;
        
        if (elapsed >= duration) {
            handleTimerCompletion();
        } else if (onTickCallback != nullptr) {
            onTickCallback();
        }
    }
}

unsigned long Timer::getCurrentTime() const {
    return millis();
}

void Timer::handleTimerCompletion() {
    state = TimerState::COMPLETED;
    
    if (onCompleteCallback != nullptr) {
        onCompleteCallback();
    }
}
