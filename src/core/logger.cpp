#include <Arduino.h>
#include <stdarg.h>
#include "logger.h"

void Logger::init() {
    if (isEnabled()) {
        Serial.begin(SERIAL_BAUD_RATE);
        while (!Serial && millis() < 5000) {
            // Wait for serial connection or timeout after 5 seconds
        }
        info("Logger initialized");
    }
}

void Logger::log(LogLevel level, const char* message) {
    if (!isEnabled()) return;
    
    Serial.print("[");
    Serial.print(millis());
    Serial.print("] ");
    Serial.print(getLevelString(level));
    Serial.print(": ");
    Serial.println(message);
}

void Logger::logf(LogLevel level, const char* format, ...) {
    if (!isEnabled()) return;
    
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    log(level, buffer);
}

void Logger::debug(const char* message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const char* message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const char* message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const char* message) {
    log(LogLevel::ERROR, message);
}

void Logger::debugf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    debug(buffer);
}

void Logger::infof(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    info(buffer);
}

void Logger::warningf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    warning(buffer);
}

void Logger::errorf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    error(buffer);
}

const char* Logger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

bool Logger::isEnabled() {
    return DEBUG_ENABLED;
}
