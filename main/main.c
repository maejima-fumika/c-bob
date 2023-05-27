#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_chip_info.h"
#include "esp_spi_flash.h"
#include <driver/gpio.h>
#include <driver/ledc.h>
#include "esp_timer.h"
#include "distance_sensor.h"
#include "motor_driver.h"
#include "car_driver.h"
#include "experiment.h"

void app_main(void)
{
    printf("Start main\n");

    // distance_sensor_tdiff_queue = xQueueCreate(10, sizeof(int64_t));
    // xTaskCreate(read_sensor_task, "read_sensor_task_0", 1024 * 2, NULL, 0, NULL);
    // xTaskCreate(drive_car_task, "read_distance_task", 2048, NULL, 10, NULL);
    ds_semphr = xSemaphoreCreateBinary();
    xTaskCreate(read_sensor_task, "read_sensor_task_0", 1024 * 2, NULL, 3, NULL);
    xTaskCreate(drive_car_task, "drive_car_task", 2048, NULL, 3, NULL);

    // init_motors();
    // set_duty(MOTOR_LEFT, 0.0);
    // set_duty(MOTOR_RIGHT, 0.0);

    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        print_average_time();
    }
}

