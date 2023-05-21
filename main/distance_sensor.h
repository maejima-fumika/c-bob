#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

void read_sensor_task(void *arg);

// extern QueueHandle_t distance_sensor_tdiff_queue;
extern float distance;
extern SemaphoreHandle_t ds_semphr;