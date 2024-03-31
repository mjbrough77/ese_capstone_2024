/**
  *@file tasks_ese.h
  *@author Mitchell Brough
  *@brief Library containing task handles used in our program
  *
  * Because task handles are only modified once during task creation,
  * global access is allowed
  *
  *@version 1.0
  *@date 2024-03-05
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#ifndef TASKS_ESE_H
#define TASKS_ESE_H

#include "FreeRTOS.h"   /* TaskHandle_t */
#include "task.h"       /* TaskHandle_t */

/*
 * Task handles should reflect the task they are being used for
 */
extern TaskHandle_t mpu_read_handle;
extern TaskHandle_t eeprom_write_handle;
extern TaskHandle_t send_boardT_handle;
extern TaskHandle_t find_velocity_left_handle;
extern TaskHandle_t find_velocity_right_handle;
extern TaskHandle_t system_error_handle;
extern TaskHandle_t find_tilt_handle;

#endif
