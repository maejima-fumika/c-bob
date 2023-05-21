#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"

extern int isr_count;
extern float isr_time_avrg;

extern int ds_measuring_count;
extern float ds_measuring_time_avrg;

extern int car_driver_count;
extern float car_driver_time_avrg;

void print_average_time(void);