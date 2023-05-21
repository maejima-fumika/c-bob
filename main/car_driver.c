#include "car_driver.h"
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "distance_sensor.h"
#include "motor_driver.h"
#include <sys/time.h>
#include "experiment.h"

running_state_t car_running_state = CAR_STOPED;
esp_timer_handle_t going_back_timer_handler;
esp_timer_handle_t turning_left_timer_handler;


static void going_back_timer_callback(void *args)
{
    if (car_running_state == CAR_GOING_BACK)
    {
        set_duty(MOTOR_LEFT, 30);
        set_duty(MOTOR_RIGHT, 90);
        car_running_state = CAR_TURNING_LEFT;
        esp_timer_start_once(turning_left_timer_handler, 2500000);
    }
}

static void turning_left_timer_callback(void *arg)
{
    if (car_running_state == CAR_TURNING_LEFT)
    {
        set_duty(MOTOR_LEFT, 50);
        set_duty(MOTOR_RIGHT, 50);
        car_running_state = CAR_GOING_STRAIGHT;
    }
}

void drive_car_task(void *arg)
{
    init_motors();

    // init timers
    const esp_timer_create_args_t going_back_timer_args = {
        .callback = &going_back_timer_callback,
        .name = "going_back_timer"
    };
    const esp_timer_create_args_t turning_left_timer_args = {
        .callback = &turning_left_timer_callback,
        .name = "turning_left_timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&going_back_timer_args, &going_back_timer_handler));
    ESP_ERROR_CHECK(esp_timer_create(&turning_left_timer_args, &turning_left_timer_handler));

    for (;;)
    {
        if (xSemaphoreTake(ds_semphr, portMAX_DELAY))
        {
            struct timeval ex_start;
            gettimeofday(&ex_start, NULL);

            if (distance < 10) {
                switch (car_running_state)
                {
                case CAR_STOPED:
                    set_duty(MOTOR_LEFT, -30);
                    set_duty(MOTOR_RIGHT, -30);
                    car_running_state = CAR_GOING_BACK;
                    esp_timer_start_once(going_back_timer_handler, 2500000);
                    break;
                case CAR_GOING_STRAIGHT:
                case CAR_TURNING_LEFT:
                    set_duty(MOTOR_LEFT, 0);
                    set_duty(MOTOR_RIGHT, 0);
                    car_running_state = CAR_STOPED;
                    break;
                default:
                    break;    
                }
            } else {
                if (car_running_state == CAR_STOPED) {
                    set_duty(MOTOR_LEFT, 50);
                    set_duty(MOTOR_RIGHT, 50);
                    car_running_state = CAR_GOING_STRAIGHT;
                }
            }

            struct timeval ex_end;
            gettimeofday(&ex_end, NULL);
            int64_t time_diff = (int64_t)ex_end.tv_sec * 1000000L + (int64_t)ex_end.tv_usec - ((int64_t)ex_start.tv_sec * 1000000L + (int64_t)ex_start.tv_usec);
            car_driver_time_avrg = car_driver_time_avrg * (float)car_driver_count / (float)(car_driver_count + 1) + (float)time_diff / (car_driver_count + 1);
            car_driver_count += 1;
        }
    }
}
