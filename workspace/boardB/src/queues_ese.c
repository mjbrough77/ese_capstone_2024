/**
  *@file queues_ese.c
  *@author Mitchell Brough
  *@brief Initializes queue handles to NULL to prevent hard faults
  *
  * Should a task or interrupt try and access an uninitialized queue, it is
  * likely to access non-accessable memory and cause a hard fault.
  *
  *@version 1.0
  *@date 2024-03-30
  *
  *@copyright Copyright (c) 2024
 */

#include "../include/queues_ese.h"

/* Safe for global use, only written once and only ever read */
QueueHandle_t i2c2Q = NULL;
QueueHandle_t mpu_dataQ = NULL;
QueueHandle_t left_wheel_dataQ = NULL;
QueueHandle_t right_wheel_dataQ = NULL;
QueueHandle_t ultrasonic_dataQ = NULL;
