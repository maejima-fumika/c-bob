#include <stdio.h>

typedef enum {
    MOTOR_LEFT = 0,
    MOTOR_RIGHT = 1,
} motor_side_t;

void init_motors(void);

void set_duty(motor_side_t side, float duty_cycle);
