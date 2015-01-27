/**
 * Buzzer driver.
 *
 * The buzzer is connected to pin A5?
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <avr/io.h>

void buzzer_init(void) {
    DDRA |= 1 << PINA5;
}

void buzzer_on(void) {
    PORTA |= 1 << PINA5;
}

void buzzer_off(void) {
    PORTA &= ~(1 << PINA5);
}

#endif
