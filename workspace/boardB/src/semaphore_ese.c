/**
  *@file semaphore_ese.c
  *@author Mitchell Brough
  *@brief Initializes semaphore handles to NULL to prevent hard faults
  *
  * Should a task or interrupt try and access an uninitialized semaphore,
  * it is likely to access non-accessable memory and cause a hard fault.
  *
  *@version 1.0
  *@date 2024-03-30
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#include "../include/semaphore_ese.h"

SemaphoreHandle_t i2c2_mutex = NULL;
