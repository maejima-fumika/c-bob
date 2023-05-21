#include <stdio.h>
#include <stdlib.h>
#include "motor_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm.h"
#include "driver/gpio.h"

// maker drive datasheet: https://docs.google.com/document/d/1XakJWz9DAtrMc_Jf75FnDXs4gwT_osCSj3Rbx1OU7bM/view

// Motor position
//
//              head
//          |---------|
//          |         |
//          |         |
//          |         |
//      |---|         |---|
// left |   |         |   | right
//      |---|_________|---|


#define M1A_GPIO_NUM GPIO_NUM_13 // Left 1
#define M1B_GPIO_NUM GPIO_NUM_5  // Left 2
#define M2A_GPIO_NUM GPIO_NUM_2  // Right 1
#define M2B_GPIO_NUM GPIO_NUM_12 // Right 2

#define LEFT_MCPWM_UNIT                0
#define LEFT_MCPWM_TIMER               0
#define RIGHT_MCPWM_UNIT               1
#define RIGHT_MCPWM_TIMER              1

#define MCPWM_FREQ_HZ             19000 // max 2000

// unit: %
#define MAX_DUTY 100
#define MIN_DUTY 5

void init_motors(void) {
    ESP_ERROR_CHECK(mcpwm_gpio_init(LEFT_MCPWM_UNIT, MCPWM0A, M1A_GPIO_NUM));
    ESP_ERROR_CHECK(mcpwm_gpio_init(LEFT_MCPWM_UNIT, MCPWM0B, M1B_GPIO_NUM));

    ESP_ERROR_CHECK(mcpwm_gpio_init(RIGHT_MCPWM_UNIT, MCPWM1A, M2A_GPIO_NUM));
    ESP_ERROR_CHECK(mcpwm_gpio_init(RIGHT_MCPWM_UNIT, MCPWM1B, M2B_GPIO_NUM));

    mcpwm_config_t pwm_config = {
        .frequency = MCPWM_FREQ_HZ,
        .cmpr_a = 0,
        .cmpr_b = 0,
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };
    ESP_ERROR_CHECK(mcpwm_init(LEFT_MCPWM_UNIT, LEFT_MCPWM_TIMER, &pwm_config));
    ESP_ERROR_CHECK(mcpwm_init(RIGHT_MCPWM_UNIT, RIGHT_MCPWM_TIMER, &pwm_config));
}

static void stop_motor(motor_side_t side) {
    mcpwm_unit_t unit = (side == MOTOR_LEFT) ? LEFT_MCPWM_UNIT : RIGHT_MCPWM_UNIT;
    mcpwm_timer_t timer = (side == MOTOR_LEFT) ? LEFT_MCPWM_TIMER : RIGHT_MCPWM_TIMER;

    mcpwm_set_signal_low(unit, timer, MCPWM_GEN_A);
    mcpwm_set_signal_low(unit, timer, MCPWM_GEN_B);
}

void set_duty(motor_side_t side, float duty_cycle) {
    mcpwm_unit_t unit = (side == MOTOR_LEFT) ? LEFT_MCPWM_UNIT : RIGHT_MCPWM_UNIT;
    mcpwm_timer_t timer = (side == MOTOR_LEFT) ? LEFT_MCPWM_TIMER : RIGHT_MCPWM_TIMER;

    if (abs(duty_cycle) < MIN_DUTY) {
        stop_motor(side);
        return;
    }
    if (abs(duty_cycle) > MAX_DUTY)
        duty_cycle = duty_cycle > 0 ? MAX_DUTY : -MAX_DUTY;

    /* motor moves in forward direction, with duty cycle = duty % */
    if (duty_cycle > 0) {
        mcpwm_set_signal_low(unit, timer, MCPWM_GEN_A);
        mcpwm_set_duty(unit, timer, MCPWM_GEN_B, duty_cycle);
        mcpwm_set_duty_type(unit, timer, MCPWM_GEN_B, MCPWM_DUTY_MODE_0);
    }
    /* motor moves in backward direction, with duty cycle = -duty % */
    else {
        mcpwm_set_signal_low(unit, timer, MCPWM_GEN_B);
        mcpwm_set_duty(unit, timer, MCPWM_GEN_A, -duty_cycle);
        mcpwm_set_duty_type(unit, timer, MCPWM_GEN_A, MCPWM_DUTY_MODE_0);
    }
}


