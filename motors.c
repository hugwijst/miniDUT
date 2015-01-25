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

void init_motors(void) {
    // Disable JTAG, as PC2-5 are otherwise not usable
    MCUCSR |= 1 << JTD;
    MCUCSR |= 1 << JTD;

    // Make sure we don't drive any of the motors
    for(int i = 0; i < 4; i++) {
        set_motor_state(i, MS_OFF);
    }

    // Configure the motor pins as output I/O
    DDRC = 0xFF;
}

/**
 * Set the state of motor `num` to `state`.
 *
 * @param num The motor to set the state of, between 0 and 3.
 * @param state The new state of the motor.
 */
void set_motor_state(int num, MotorState state) {
    // Disable both pins
    PORTC &= ~(0x3 << (num * 2));

    switch (state) {
        case MS_OFF: /* nothing to do */ break;
        case MS_FORWARD: PORTC |= (0x1 << (num * 2)); break;
        case MS_BACKWARD: PORTC |= (0x2 << (num * 2)); break;
    }
}
