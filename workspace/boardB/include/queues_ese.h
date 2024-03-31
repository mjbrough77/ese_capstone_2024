/**
  *@file queues_ese.h
  *@author Mitchell Brough
  *@brief Library containing the handles for queues used in our program
  *
  * Because queue handles are only modified once during initialization,
  * global access is allowed
  *
  *@version 1.0
  *@date 2024-03-30
  *
  *@copyright Copyright (c) 2024
 */

#ifndef QUEUES_ESE_H
#define QUEUES_ESE_H

#include "FreeRTOS.h"   /* QueueHandle_t */
#include "queue.h"      /* QueueHandle_t */

/**
  *@brief Queue for storing data sent over I2C2
  *
  * This queue only stores data which is *not* sent by the DMA controller
  * (i.e., device addresses, data which can't be sent by DMA)
 */
extern QueueHandle_t i2c2Q;

/**
  *@brief Queue for storing raw MPU6050 data
  *
  * Acts as a "mailbox" so any old MPU6050 data will be overwritten the next
  * time this queue is written to
 */
extern QueueHandle_t mpu_dataQ;

/**
  *@brief Queue for storing converted encoder data into a speed (left wheel)
  *
  * Acts as a "mailbox" so any old speed data will be overwritten the next
  * time this queue is written to
 */
extern QueueHandle_t left_wheel_dataQ;

/**
  *@brief Queue for storing converted encoder data into a speed (right wheel)
  *
  * Acts as a "mailbox" so any old speed data will be overwritten the next
  * time this queue is written to
 */
extern QueueHandle_t right_wheel_dataQ;

/**
  *@brief Queue for storing distance readings from ultrasonic sensors
  *
  * This queue is currently only used for the log, boardB does not process this
  * data. Acts as a "mailbox" so any old distance data will be overwritten the
  * next time this queue is written to
 */
extern QueueHandle_t ultrasonic_dataQ;

#endif
