#ifndef TASKS_ESE_H
#define TASKS_ESE_H

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t motor_control_handle;
extern TaskHandle_t display_handle;
extern TaskHandle_t ultrasonic_handle;

#endif
