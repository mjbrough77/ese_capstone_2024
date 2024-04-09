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
extern TaskHandle_t motor_control_handle;
extern TaskHandle_t display_handle;
extern TaskHandle_t ultrasonic_handle;

#endif
