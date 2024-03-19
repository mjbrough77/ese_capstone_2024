/**
  *@file tasks_ese.h
  *@author Mitchell Brough
  *@brief Library holding globally accessable task handles
  *
  * We have enabled co-operative scheduling to better control reentrancy
  * See RTE/RTOS/FreeRTOSConfig.h
  *
  *@version 1.0
  *@date 2024-03-05
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */

#ifndef TASKS_ESE_H
#define TASKS_ESE_H

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t mpu_read_handle;
extern TaskHandle_t eeprom_write_handle;
extern TaskHandle_t send_speed_handle;

#endif
