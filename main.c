#include <stdbool.h>

#include <stdlib.h>

#include "button.h"
#include "buzzer.h"
#include "display.h"
#include "lcd.h"
#include "motors.h"
#include "timer.h"
#include "tsal.h"
#include "util.h"

// Buttons: PA0-3
#define BTN_SWITCH_HV PINA, PINA1
#define BTN_PRECHARGE PINA, PINA2
#define BTN_START     PINA, PINA3

#define BTN_NUM_SWITCH_HV 0
#define BTN_NUM_PRECHARGE 1
#define BTN_NUM_START     2

typedef enum {
    ST_IDLE,
    ST_PRECHARGE,
    ST_READY_TO_DRIVE,
    ST_DRIVE,
    ST_ERROR,
} State;

const char *motor_names[4] = {"FL", "FR", "RL", "RR"};
bool motor_errors[4] = {false, false, false, false};

void error(const char* message);


void to_idle(State old_state) {
    buzzer_off();
    for (int i = 0; i < 4; i++) {
        motor_set_state(i, MS_OFF);
    }
}

void to_precharge(State old_state) {
    tsal_off();
    buzzer_off();

    if (!gpio_val(BTN_SWITCH_HV)) {
        error("Can't start precharging when the tractive switch is set to 'off'.");
        return;
    }

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

bool _drive_buzzer_sounded = true;

void to_drive(State old_state) {
    int rn = time_ms() % (10000);
    _drive_buzzer_sounded = false;
    for (int i = 0; i < 4; i++) {
        MotorState ms;
        int motor_rn = rn % 10;
        if      (motor_rn < 6) { ms = MS_FORWARD;  motor_errors[i] = false; }
        else if (motor_rn < 7) { ms = MS_BACKWARD; motor_errors[i] = true; }
        else                   { ms = MS_OFF;      motor_errors[i] = true; }

        motor_set_state(i, ms);
        rn /= 10;
    }
}

void to_error(State old_state) {
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
        case ST_PRECHARGE:          to_precharge     (old_state); break;
        case ST_READY_TO_DRIVE:     to_ready_to_drive(old_state); break;
        case ST_DRIVE:              to_drive         (old_state); break;
        case ST_ERROR:              to_error         (old_state); break;
    }
}

const char * _error_str = 0;
void error(const char* message) {
    //_error_str = message;
    display_center_string(1, message);
    to_state(ST_ERROR);
}

void display_run2(void) {
    const char * state_str = "----";
    switch (_state) {
        case ST_IDLE:           state_str = "Standby"; break;
        case ST_PRECHARGE:      state_str = "Precharging"; break;
        case ST_READY_TO_DRIVE: state_str = "Precharged"; break;
        case ST_DRIVE:          state_str = "Driving"; break;
        case ST_ERROR:          state_str = "Error"; break;
    }
    //lcd_set_location(0, 0);
    //lcd_send_string(state_str);
    display_center_string(0, state_str);

    char buf[17] = "";
    switch (_state) {
        case ST_IDLE: {
            display_center_string(1, buf);
            break;
        }
        case ST_PRECHARGE: {
            // x/16 part of the time that has passed
            uint16_t slice = (time_ms() - _time_at_state_change) / 353; // 353 = 6000 / 17 rounded down
            int i;
            for(i = 0; i < slice && i < 16; i++) {
                buf[i] = 0xff;
            }
            for(; i < 16; i++) {
                buf[i] = ' ';
            }
            buf[16] = '\0';
            display_center_string(1, buf);
            break;
        }
        case ST_READY_TO_DRIVE:  {
            display_center_string(1, buf);
            break;
        }
        case ST_DRIVE: {
            bool has_error = false;
            for (int i = 0; i < 4; i++) {
                if (motor_errors[i]) {
                    has_error = true;
                }
            }
            if (has_error) {
                strcpy(buf, "2310:");
                for (int i = 0; i < 4; i++) {
                    if (motor_errors[i]) {
                        strncat(buf, " ", 16);
                        strncat(buf, motor_names[i], 16);
                    }
                }
                buf[16] = '\0';
            }
            display_center_string(1, buf);
            break;
        }
        case ST_ERROR:          break;
    }
    //lcd_send_chars(buf, 16);
}

int main(void) {
    MCUCSR |= 1 << JTD;
    MCUCSR |= 1 << JTD;
    // initialize uc
    buzzer_init();
    lcd_init();
    motors_init();
    timer_init();
    tsal_init();

    // initialize buttons
    btn_init(BTN_SWITCH_HV);
    btn_init(BTN_START);
    btn_init(BTN_PRECHARGE);

    to_idle(_state);

    for(;;) {
        // run the TSAL logic every iteration
        motors_run();
        tsal_run();
        display_run();
        display_run2();

        // perform state action
        switch(_state) {
            case ST_IDLE:
                if (time_ms() - _time_at_state_change > 3000) {
                    tsal_off();
                }
                if ( btn_pressed(BTN_NUM_PRECHARGE, BTN_PRECHARGE) ) {
                    to_state(ST_PRECHARGE);
                    continue;
                }
                if ( btn_pressed(BTN_NUM_START, BTN_START) ) {
                    error("Motor controllers did not enable.");
                }
                break;

            case ST_PRECHARGE:
                if ( btn_pressed(BTN_NUM_PRECHARGE, BTN_PRECHARGE) ) {
                    to_state(ST_IDLE);
                    continue;
                }
                if ( btn_pressed(BTN_NUM_START, BTN_START) ) {
                    error("Motor controllers did not enable.");
                    continue;
                }

                if ( time_ms() - _time_at_state_change > 2000 ) {
                    tsal_on();
                }
                if ( time_ms() - _time_at_state_change > 6000 ) {
                    to_state(ST_READY_TO_DRIVE);
                }
                break;

            case ST_READY_TO_DRIVE:
                if ( btn_pressed(BTN_NUM_PRECHARGE, BTN_PRECHARGE) ) {
                    to_state(ST_IDLE);
                    continue;
                }
                if ( btn_pressed(BTN_NUM_START, BTN_START) ) {
                    to_state(ST_DRIVE);
                    continue;
                }
                if (!gpio_val(BTN_SWITCH_HV)) {
                    error("Lost High Voltage.");
                    continue;
                }
                break;

            case ST_DRIVE: {
                if (_drive_buzzer_sounded) {
                    /* nothing */
                } else if ( time_ms() - _time_at_state_change < 750 ) {
                    buzzer_on();
                } else if ( time_ms() - _time_at_state_change < 1000 ) {
                    buzzer_off();
                } else if ( time_ms() - _time_at_state_change < 2000 ) {
                    buzzer_on();
                } else {
                    buzzer_off();
                    _drive_buzzer_sounded = true;
                }

                if ( btn_pressed(BTN_NUM_PRECHARGE, BTN_PRECHARGE)
                        || btn_pressed(BTN_NUM_START, BTN_START) ) {
                    to_state(ST_IDLE);
                    continue;
                }
                if (!gpio_val(BTN_SWITCH_HV)) {
                    error("Lost High Voltage.");
                    continue;
                }
                break;
            }

            case ST_ERROR:
                if (time_ms() - _time_at_state_change > 3000) {
                    tsal_off();
                }
                if ( btn_pressed(BTN_NUM_START, BTN_START)
                        || btn_pressed(BTN_NUM_PRECHARGE, BTN_PRECHARGE) ) {
                    to_state(ST_IDLE);
                }
                break;

        }
    }
}
