#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

inline void timer_init(void) {
    // Set the prescaler to TotalFrequency / 1024.
    // This will give an increment approximately every 1ms.
    TCCR1B |= 1 << CS12 | 1 << CS10;
}

// The current time, in ms
// `init_timer` has to be called before this method will work
inline uint16_t time_ms(void) {
    return TCNT1;
}

#endif

