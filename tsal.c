#include "tsal.h"

#include <avr/io.h>

#include "timer.h"

bool _is_tsal_on = false;

void tsal_on(void) {
    _is_tsal_on = true;
}

void tsal_off(void) {
    _is_tsal_on = false;
}

void tsal_run(void) {
    if(_is_tsal_on && (get_time() / 200) % 2 == 0) {
        // enable TSAL LED
        PORTD |= 0x1;
    } else {
        // disable TSAL LED
        PORTD &= ~0x1;
    }
}
