#include "../include/queues_ese.h"

/* Safe for global use, only written once and only ever read */
QueueHandle_t i2c2Q = NULL;
QueueHandle_t mpu_dataQ = NULL;
QueueHandle_t left_wheel_dataQ = NULL;
QueueHandle_t right_wheel_dataQ = NULL;
QueueHandle_t ultrasonic_dataQ = NULL;
