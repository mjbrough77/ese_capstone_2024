/**
  * @file motor_ese.h
  * @author Mitchell Brough
  * @brief Function prototypes related to motor driver communication/control
  * @version 1.0
  * @date 2024-04-09
  *
  * @copyright Copyright (c) 2024 Mitchell Brough
 */

#ifndef MOTOR_ESE_H
#define MOTOR_ESE_H

/**
  * @brief Updates the motor driver signals
  *
  * Task will unblock on every new notification recieved. Notifications
  * could be over USART or on the next motor signal period.
  *
  * This task keeps count of all sensor warnings in the system, and as such
  * we are assuming that stop, slow, and clear notifcations are sent only
  * once as the sensor enters/exits the warning state
  *
  * @param param unused
 */
_Noreturn void motor_control_task(void* param);

#endif
