#ifndef TIMER_H
#define TIMER_H

class Timer {
    public: 
        void start(unsigned long durationMs);
        void stop();
        bool isRunning();
        unsigned long getRemaining();
        float getFractionalRemaining();
    
    private:
        unsigned long startTime;
        unsigned long duration;
        bool running;
}; 

#endif