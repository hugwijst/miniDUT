/**
 * Motor control driver.
 *
 * Motors are controlled by driving a pin high (motor enabled) or low (motor
 * disabled).
 * 
 * The motor enabled pins are:
 * PC0-1: front left
 * PC2-3: front right
 * PC4-5: rear left
 * PC6-7: rear right
 */

#include "motors.h"

#include <avr/io.h>

#include "timer.h"

static MotorState _states[4] = {MS_OFF, MS_OFF, MS_OFF, MS_OFF};

/**
 * Actuate the motor `num` to `state`.
 *
 * @param num The motor to set the state of, between 0 and 3.
 * @param state The new state of the motor.
 */
void motor_actuate(int num, MotorState state) {
    // Disable both pins
    PORTC &= ~(0x3 << (num * 2));

    switch (state) {
        case MS_OFF: /* nothing to do */ break;
        case MS_FORWARD: PORTC |= (0x1 << (num * 2)); break;
        case MS_BACKWARD: PORTC |= (0x2 << (num * 2)); break;
    }
}

void motors_init(void) {
    // Disable JTAG, as PC2-5 are otherwise not usable
    MCUCSR |= 1 << JTD;
    MCUCSR |= 1 << JTD;

    // Make sure we don't drive any of the motors
    for(int i = 0; i < 4; i++) {
        motor_set_state(i, MS_OFF);
    }

    // Configure the motor pins as output I/O
    DDRC = 0xFF;
}

void motors_run(void) {
    static const uint16_t PERIOD_MS = 100;
    static const uint16_t HIGH_MS = 35;

    uint16_t cur_time = time_ms();
    for (int i = 0; i < 4; i++) {
        if( (cur_time % PERIOD_MS) < HIGH_MS ) {
            motor_actuate(i, _states[i]);
        } else {
            motor_actuate(i, MS_OFF);
        }
    }
}

/**
 * Set the state of motor `num` to `state`.
 *
 * @param num The motor to set the state of, between 0 and 3.
 * @param state The new state of the motor.
 */
void motor_set_state(int num, MotorState state) {
    if(num < 4) {
        _states[num] = state;
    }
}
