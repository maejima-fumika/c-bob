typedef enum
{
    CAR_STOPED,
    CAR_GOING_STRAIGHT,
    CAR_TURNING_LEFT,
    CAR_GOING_BACK,
} running_state_t;


void drive_car_task(void *arg);

