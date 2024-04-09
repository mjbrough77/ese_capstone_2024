/**
  *@file tasks_ese.c
  *@author Mitchell Brough
  *@brief Initializes task handles to NULL
  *@version 1.0
  *@date 2024-03-30
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#include "../include/tasks_ese.h"

/* Safe for global use, only written once and only ever read */
TaskHandle_t motor_control_handle = NULL;
TaskHandle_t display_handle = NULL;
TaskHandle_t ultrasonic_handle = NULL;
