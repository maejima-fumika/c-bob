#include <stdio.h>
#include "distance_sensor.h"
#include "driver/gpio.h"
#include <sys/time.h>
#include <unistd.h>
#include "freertos/task.h"
#include "experiment.h"

// The target distance sensor is HC-SR04
// This source code is based on the datasheet:
//         https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf

static const char *TAG = "distanc-sensor";

#define TRIG_GPIO_NUM GPIO_NUM_21
#define ECHO_GPIO_NUM GPIO_NUM_22

#define GPIO_HIGH 1
#define GPIO_LOW 0
#define ESP_INTR_FLAG_DEFAULT 0

float distance = 0;
SemaphoreHandle_t ds_semphr;

SemaphoreHandle_t time_isr_semphr;
static bool has_raised = false;
static struct timeval echo_rasing_edge_time;
static int64_t time_diff;

static int64_t ex_time_diff;

static void echo_edge_handler(void *arg)
{
    struct timeval ex_start;
    gettimeofday(&ex_start, NULL);
    if (!has_raised) {
        gettimeofday(&echo_rasing_edge_time, NULL);
        has_raised = true;
    } else {
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);
        // 時間差を取る。
        time_diff = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec - ((int64_t)echo_rasing_edge_time.tv_sec * 1000000L + (int64_t)echo_rasing_edge_time.tv_usec);
        has_raised = false;
        xSemaphoreGiveFromISR(time_isr_semphr, NULL);
    }
    struct timeval ex_end;
    gettimeofday(&ex_end, NULL);
    ex_time_diff = (int64_t)ex_end.tv_sec * 1000000L + (int64_t)ex_end.tv_usec - ((int64_t)ex_start.tv_sec * 1000000L + (int64_t)ex_start.tv_usec);
}

void read_sensor_task(void *arg)
{
    // Config gpio
    gpio_config_t trig_gpio_config = {
        .pin_bit_mask = 1ULL << TRIG_GPIO_NUM,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&trig_gpio_config);
    gpio_set_level(TRIG_GPIO_NUM, GPIO_LOW);

    gpio_config_t echo_gpio_config = {
        .pin_bit_mask = 1ULL << ECHO_GPIO_NUM,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&echo_gpio_config);

    // Add handler
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(ECHO_GPIO_NUM, echo_edge_handler, (void *)ECHO_GPIO_NUM);

    // init Semaphore
    time_isr_semphr = xSemaphoreCreateBinary();

    while (true)
    {
        struct timeval ex_start;
        gettimeofday(&ex_start, NULL);

        // 初期化
        has_raised = false;
        gpio_set_level(TRIG_GPIO_NUM, GPIO_LOW);
        usleep(10);
    
        // 信号を送信
        gpio_set_level(TRIG_GPIO_NUM, GPIO_HIGH);
        usleep(10);
        gpio_set_level(TRIG_GPIO_NUM, GPIO_LOW);

        // 割り込み処理が全部終わるまで待つ。
        xSemaphoreTake(time_isr_semphr, portMAX_DELAY);


        isr_time_avrg = isr_time_avrg * (float)isr_count / (float)(isr_count + 1) + (float)ex_time_diff / (isr_count + 1);
        isr_count += 1;

        distance = (float)time_diff * 340 * 100 / 1000000 / 2;
        // printf("distance: %f \n", distance);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // 距離の測定が終わったことを通知
        xSemaphoreGive(ds_semphr);

        struct timeval ex_end;
        gettimeofday(&ex_end, NULL);
        int64_t ex_ds_time_diff = (int64_t)ex_end.tv_sec * 1000000L + (int64_t)ex_end.tv_usec - ((int64_t)ex_start.tv_sec * 1000000L + (int64_t)ex_start.tv_usec);
        ds_measuring_time_avrg = ds_measuring_time_avrg * (float)ds_measuring_count / (float)(ds_measuring_count + 1) + (float)ex_ds_time_diff / (ds_measuring_count + 1);
        ds_measuring_count += 1;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
