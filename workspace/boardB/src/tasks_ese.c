#include "../include/tasks_ese.h"

/* Safe for global use, only written once and only ever read */
TaskHandle_t mpu_read_handle = NULL;
TaskHandle_t eeprom_write_handle = NULL;
TaskHandle_t send_speed_handle = NULL;
TaskHandle_t find_velocity_left_handle = NULL;
TaskHandle_t find_velocity_right_handle = NULL;
