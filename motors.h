#ifndef MOTORS_H
#define MOTORS_H

typedef enum {
    MS_OFF,
    MS_FORWARD,
    MS_BACKWARD,
} MotorState;

void init_motors(void);
void set_motor_state(int num, MotorState state);

#endif
