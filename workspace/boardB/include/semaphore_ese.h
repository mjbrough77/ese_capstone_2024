/**
  *@file semaphore_ese.h
  *@author Mitchell Brough
  *@brief Library containing the handles for semaphores used in our program
  *
  * Because semaphore handles are only modified once during initialization,
  * global access is allowed
  *
  *@version 1.0
  *@date 2024-03-30
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#ifndef SEMAPHORE_ESE_H
#define SEMAPHORE_ESE_H

#include "FreeRTOS.h"   /* SemaphoreHandle_t */
#include "semphr.h"     /* SemaphoreHandle_t */

/**
  *@brief Mutex to allow for mutual access to the I2C bus
  *
  * Note that mutexes may only be shared between tasks and not interrupts
 */
extern SemaphoreHandle_t i2c2_mutex;

#endif
