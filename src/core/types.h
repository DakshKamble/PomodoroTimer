#ifndef TYPES_H
#define TYPES_H

// Timer states
enum class TimerState {
    STOPPED,
    RUNNING,
    PAUSED,
    COMPLETED
};

// Pomodoro session types
enum class SessionType {
    WORK,
    SHORT_BREAK,
    LONG_BREAK
};

// Animation types
enum class AnimationType {
    COUNTDOWN,
    PULSE,
    COMET,
    SOLID_COLOR,
    TIME_SELECTION,
    FLASH_COMPLETE,
    FLASH_CANCELLED,
    OFF
};

// Application states
enum class AppState {
    TIME_SELECTION,
    COUNTDOWN_RUNNING,
    TIMER_COMPLETE,
    TIMER_CANCELLED
};

// Error codes
enum class ErrorCode {
    SUCCESS = 0,
    INVALID_DURATION,
    TIMER_NOT_RUNNING,
    HARDWARE_ERROR,
    MEMORY_ERROR
};

#endif
