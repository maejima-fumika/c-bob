#include "experiment.h"

int isr_count;
float isr_time_avrg; // usec

int ds_measuring_count;
float ds_measuring_time_avrg; 

int car_driver_count;
float car_driver_time_avrg;

void print_average_time(void) {
    printf("Isr time average: %f\n", isr_time_avrg);
    printf("Distance Sensor time average: %f\n", ds_measuring_time_avrg);
    printf("Car driver time average: %f\n", car_driver_time_avrg);

    // 初期化
    isr_count = 0;
    isr_time_avrg = 0;
    ds_measuring_count = 0;
    ds_measuring_time_avrg = 0;
    car_driver_count = 0;
    car_driver_time_avrg = 0;
}