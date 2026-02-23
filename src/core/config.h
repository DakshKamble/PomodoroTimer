#ifndef CONFIG_H
#define CONFIG_H

// Hardware Configuration
#define NUM_LEDS 12
#define LED_PIN D7
#define LED_BRIGHTNESS 50

// Rotary Encoder Configuration
#define ENCODER_SW_PIN D2
#define ENCODER_DT_PIN D1
#define ENCODER_CLK_PIN D0
#define ENCODER_DEBOUNCE_MS 50
#define ENCODER_LONG_PRESS_MS 3000    // 3 seconds for long press

// OLED Display Configuration
#define OLED_SDA_PIN D4
#define OLED_SCL_PIN D5
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// Timing Configuration (in milliseconds)
#define POMODORO_WORK_DURATION 1500000    // 25 minutes
#define POMODORO_SHORT_BREAK 300000       // 5 minutes
#define POMODORO_LONG_BREAK 900000        // 15 minutes
#define ANIMATION_INTERVAL 16

// Debug Configuration
#define SERIAL_BAUD_RATE 115200
#define DEBUG_ENABLED true

// Timer Selection Configuration
#define MAX_TIMER_MINUTES 60              // Maximum timer setting (60 minutes = 1 hour)
#define TIMER_STEP_MINUTES 5              // Step size for timer adjustment (5 minutes per step)
#define ENCODER_STEPS_PER_INCREMENT 3     // Number of encoder steps needed for one timer increment
#define FLASH_ANIMATION_CYCLES 3          // Number of flash cycles at completion

// Default test duration (for development)
#define TEST_COUNTDOWN_DURATION 30000     // 30 seconds

#endif
