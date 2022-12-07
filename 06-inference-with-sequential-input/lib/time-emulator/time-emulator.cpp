#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include <unistd.h>

#include "time-emulator.h"

// Sleeps the program by the number of milliseconds specified
// Note: Linux is NOT (by default) a real-time OS. Delay is best effort.
void delay(unsigned long ms) {
    struct timespec req, rem;
    req.tv_sec = 0;
    req.tv_nsec = 500000;

    // Wake up every half millisecond to check timer
    unsigned long t_start = millis();
    while (millis() - t_start < ms) {
        nanosleep(&req, &rem);
    }
}

// Sleeps the program by the number of microseconds specified
// Note: Linux is NOT (by default) a real-time OS. Delay is best effort.
void delayMicroseconds(unsigned long us) {
    struct timespec req, rem;
    req.tv_sec = 0;
    req.tv_nsec = 500;

    // Wake up every half microsecond to check timer
    unsigned long t_start = micros();
    while (micros() - t_start < us) {
        nanosleep(&req, &rem);
    }
}

// Return elapsed time in microseconds
unsigned long micros(void) {
    struct timespec time_now;
    if (clock_gettime(CLOCK_MONOTONIC, &time_now) == -1) {
        return 0;
    }
    return time_now.tv_sec * 1000000 + time_now.tv_nsec / 1e3;
}

// Return elapsed time in milliseconds
unsigned long millis(void) {
    struct timespec time_now;
    if (clock_gettime(CLOCK_MONOTONIC, &time_now) == -1) {
        return 0;
    }
    return time_now.tv_sec * 1000 + time_now.tv_nsec / 1e6;
}



