#include "button.h"
#include "buzzer.h"
#include "lcd.h"
#include "motors.h"
#include "timer.h"
#include "tsal.h"

// Buttons: PA0-3
#define BTN_SWITCH_HV 0, PORTA, PIN1
#define BTN_START     1, PORTA, PIN2
#define BTN_PRECHARGE 2, PORTA, PIN3

#define BTN_DEBOUNCE_TIME 100

typedef enum {
    ST_IDLE,
    ST_PRECHARGE,
    ST_READY_TO_DRIVE,
    ST_DRIVE,
    ST_ERROR,
} State;

void to_idle(State old_state) {
    tsal_off();
    buzzer_off();
    for (int i = 0; i < 4; i++) {
        motor_set_state(i, MS_OFF);
    }
}

void to_pre_charge(State old_state) {
    tsal_off();
    buzzer_off();
    for (int i = 0; i < 4; i++) {
        motor_set_state(i, MS_OFF);
    }
}

void to_ready_to_drive(State old_state) {
    tsal_on();
    buzzer_off();
    for (int i = 0; i < 4; i++) {
        motor_set_state(i, MS_OFF);
    }
}

void to_drive(State old_state) {
    int rn = time_ms() % 12;
    // algorithm to determine the motors to drive
    //drive_motors();
    buzzer_on();
}

void to_error(State old_state) {
    tsal_off();
    buzzer_off();
    for (int i = 0; i < 4; i++) {
        motor_set_state(i, MS_OFF);
    }
}

State _state = ST_IDLE;
uint16_t _time_at_state_change;

void to_state(State new_state) {
    State old_state = _state;
    _state = new_state;
    _time_at_state_change = time_ms();

    switch(_state) {
        default:  /* should not be reached, fall through to ST_IDLE */
            _state = ST_IDLE;

        case ST_IDLE:               to_idle          (old_state); break;
        case ST_PRECHARGE:          to_pre_charge    (old_state); break;
        case ST_READY_TO_DRIVE:     to_ready_to_drive(old_state); break;
        case ST_DRIVE:              to_drive         (old_state); break;
        case ST_ERROR:              to_error         (old_state); break;
    }
}

void display_run(void) {
    const char * state_str = "      ----      ";
    switch (_state) {
        case ST_IDLE:           state_str = "    Standby     "; break;
        case ST_PRECHARGE:      state_str = "  Precharging   "; break;
        case ST_READY_TO_DRIVE: state_str = "   Precharged   "; break;
        case ST_DRIVE:          state_str = "    Driving     "; break;
        case ST_ERROR:          state_str = "     Error      "; break;
    }
    lcd_set_location(0, 0);
    lcd_send_string(state_str);

    //switch (_state) {
    //}
}

int main(void) {
    // initialize uc
    buzzer_init();
    lcd_init();
    motors_init();
    timer_init();

    to_idle(_state);

    for(;;) {
        // run the TSAL logic every iteration
        tsal_run();
        display_run();

        // perform state action
        switch(_state) {
            case ST_IDLE:
                if ( btn_pressed(BTN_PRECHARGE, BTN_DEBOUNCE_TIME) ) {
                    to_state(ST_PRECHARGE);
                }
                break;

            case ST_PRECHARGE:
                if ( time_ms() - _time_at_state_change > 2000 ) {
                    tsal_on();
                }
                if ( time_ms() - _time_at_state_change > 6000 ) {
                    to_state(ST_READY_TO_DRIVE);
                }
                break;

            case ST_READY_TO_DRIVE:
                if ( btn_pressed(BTN_PRECHARGE, BTN_DEBOUNCE_TIME) ) {
                    to_state(ST_IDLE);
                }
                if ( btn_pressed(BTN_START, BTN_DEBOUNCE_TIME) ) {
                    to_state(ST_DRIVE);
                }
                break;

            case ST_DRIVE:
                if ( time_ms() - _time_at_state_change > 1337 ) {
                    buzzer_off();
                }
                if ( btn_pressed(BTN_PRECHARGE, BTN_DEBOUNCE_TIME) ) {
                    to_state(ST_IDLE);
                }
                if ( btn_pressed(BTN_START, BTN_DEBOUNCE_TIME) ) {
                    to_state(ST_PRECHARGE);
                }
                break;

            case ST_ERROR:
                if ( btn_pressed(BTN_START, BTN_DEBOUNCE_TIME) ) {
                    to_state(ST_IDLE);
                }
                if ( btn_pressed(BTN_PRECHARGE, BTN_DEBOUNCE_TIME) ) {
                    //error_string = "IN ERROR MODE, PRESS START TO IGNORE";
                }
                break;

        }
    }
}
