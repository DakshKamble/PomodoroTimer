#ifndef LOGGER_H
#define LOGGER_H

#include "config.h"

// Log levels
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3
};

class Logger {
public:
    static void init();
    static void log(LogLevel level, const char* message);
    static void logf(LogLevel level, const char* format, ...);
    
    // Convenience methods
    static void debug(const char* message);
    static void info(const char* message);
    static void warning(const char* message);
    static void error(const char* message);
    
    static void debugf(const char* format, ...);
    static void infof(const char* format, ...);
    static void warningf(const char* format, ...);
    static void errorf(const char* format, ...);

private:
    static const char* getLevelString(LogLevel level);
    static bool isEnabled();
};

// Macros for easier logging (only compile in debug builds)
#if DEBUG_ENABLED
    #define LOG_DEBUG(msg) Logger::debug(msg)
    #define LOG_INFO(msg) Logger::info(msg)
    #define LOG_WARNING(msg) Logger::warning(msg)
    #define LOG_ERROR(msg) Logger::error(msg)
    
    #define LOG_DEBUGF(fmt, ...) Logger::debugf(fmt, __VA_ARGS__)
    #define LOG_INFOF(fmt, ...) Logger::infof(fmt, __VA_ARGS__)
    #define LOG_WARNINGF(fmt, ...) Logger::warningf(fmt, __VA_ARGS__)
    #define LOG_ERRORF(fmt, ...) Logger::errorf(fmt, __VA_ARGS__)
#else
    #define LOG_DEBUG(msg)
    #define LOG_INFO(msg)
    #define LOG_WARNING(msg)
    #define LOG_ERROR(msg)
    
    #define LOG_DEBUGF(fmt, ...)
    #define LOG_INFOF(fmt, ...)
    #define LOG_WARNINGF(fmt, ...)
    #define LOG_ERRORF(fmt, ...)
#endif

#endif
