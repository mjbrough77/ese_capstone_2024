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
TaskHandle_t mpu_read_handle            = NULL;
TaskHandle_t eeprom_write_handle        = NULL;
TaskHandle_t send_boardT_handle         = NULL;
TaskHandle_t find_velocity_left_handle  = NULL;
TaskHandle_t find_velocity_right_handle = NULL;
TaskHandle_t system_error_handle        = NULL;
TaskHandle_t find_tilt_handle           = NULL;
