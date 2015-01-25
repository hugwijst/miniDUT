#ifndef MOTORS_H
#define MOTORS_H

typedef enum {
    MS_OFF,
    MS_FORWARD,
    MS_BACKWARD,
} MotorState;

void motors_init(void);
void motor_set_state(int num, MotorState state);

#endif
