/**
  *@file timers_ese.h
  *@author Mitchell Brough
  *@brief Function prototypes for hardware timers and sensors making use of
  *        hardware timers
  *
  *@version 1.0
  *@date 2024-04-09
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#ifndef TIMERS_ESE_H
#define TIMERS_ESE_H

#include "stdint.h" /* uint16_t */

/**
  *@brief Sets up TIM2 to be used as the TRIG pin for the ultrasonics
  *
  * TIM2 acts as the master timer with TIM4 as its slave, configured so that
  * TIM4 will reset on every update of the TIM2 signal (TRIG sent)
  *
  *@pre TIM2 clock is enabled
  *@pre AHB clock is 20MHz
 */
void configure_tim2(void);

/**
  *@brief Sets up TIM3 to be used as the S1 and S2 inputs for the motor driver
  *
  *@pre TIM3 clock is enabled
  *@pre AHB clock is 20MHz
 */
void configure_tim3(void);

/**
  *@brief Sets up TIM4 for input capture of echo pins on the ultrasonics
  *
  * TIM4 is reset on every update of the TRIG pin
  *
  *@pre TIM4 clock is enabled
  *@pre AHB clock is 20MHz
 */
void configure_tim4(void);

/**
  *@brief Enables TIM2 interrupt, enables TIM4, TIM2 counters
  *
  * TIM4 must start before TIM2 because TIM4 is reset after every TIM2 update
 */
void start_ultrasonics(void);

/**
  *@brief Gets last recorded timer value for the right ultrasonic
  *
  *@return uint16_t TIM4 capture/compare channel 2
 */
uint16_t read_right_ultrasonic(void);

/**
  *@brief Gets last recorded timer value for the left ultrasonic
  *
  *@return uint16_t TIM4 capture/compare channel 1
 */
uint16_t read_left_ultrasonic(void);

/**
  *@brief Converts timer data into distance data an notifies motor controller
  *
  * Utilizes hysteresis to prevent jittery speed changes
  *
  * Like all tasks monitoring sensor data, notifications should only be sent
  * during state transitions. This task contains an extra state in the case
  * of a motor slow condition
  *
  * Transfer of distance data MUST take < 60ms, Otherwise, DMA controller has
  * concurrent access to `readings`
  *
  *@param param unused
 */
_Noreturn void ultrasonic_data_task(void* param);

#endif
