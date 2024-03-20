#ifndef QUEUES_ESE_H
#define QUEUES_ESE_H

#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t i2c2Q;
extern QueueHandle_t mpu_dataQ;
extern QueueHandle_t left_wheel_dataQ;
extern QueueHandle_t right_wheel_dataQ;
extern QueueHandle_t ultrasonic_dataQ;

#endif
