#include <Arduino.h>
#include "core/timer.h"

void Timer::start(unsigned long durationMs) {
    if(durationMs == 0)
        return;
    duration = durationMs;
    startTime = millis();
    running = true;
}

void Timer::stop() {
    running = false;
}

bool Timer::isRunning() {
    return running;
}

unsigned long Timer::getRemaining() {
    if(!running)
        return 0;
    
    unsigned long elapsed = millis() - startTime;

    if(elapsed >= duration) {
        running = 0;
        return 0;
    }

    return duration - elapsed;
}

float Timer::getFractionalRemaining() {
    if(!running)
        return 0;
    
    float fraction = (float)getRemaining()/ duration;

    return fraction;

}
