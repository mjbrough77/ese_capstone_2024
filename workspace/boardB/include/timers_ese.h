/**
  *@file timers_ese.h
  *@author Mitchell Brough
  *@brief Library concerned with the use of STM32F103RB hardware timers
  *@version 1.0
  *@date 2024-03-05
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#ifndef TIMERS_ESE_H
#define TIMERS_ESE_H

/**
  *@brief Configures TIM1 as input capture of z-phase of right encoder
  *
  * TIM1 counter is *not* enabled when this function exits
  *
  *@pre TIM1 clock is enabled
  *@pre AHB running at 20MHz
 */
void configure_tim1(void);

/**
  *@brief Configures TIM2 for encoder input of the right encoder
  *
  * TIM2 counter is *not* enabled when this function exits
  *
  *@pre TIM2 clock is enabled
 */
void configure_tim2(void);

/**
  *@brief Configures TIM3 for encoder input of the left encoder
  *
  * TIM3 counter is *not* enabled when this function exits
  *
  *@pre TIM3 clock is enabled
 */
void configure_tim3(void);

/**
  *@brief Configures TIM4 as input capture of z-phase of left encoder
  *
  * TIM4 counter is *not* enabled when this function exits
  *
  *@pre TIM4 clock is enabled
  *@pre AHB clock is 20MHz
 */
void configure_tim4(void);

/**
  *@brief Starts counter on TIM1-4
 */
void start_encoder_readings(void);

/**
  *@brief Calculates the velocity of a wheel based on encoder data
  *
  * This task is unblocks when the timer CC interrupt executes
  * (i.e., when the z-phase goes high on an encoder)
  *
  * This task is used twice, one for each wheel, the timers parameter helps
  * determine which wheel is assigned to whichever task.
  *
  * The explanation below gives detailed information about the data formatting
  * of velocity. Encoder mode on the timers gives a signed representation of
  * the speed (e.g., wheels in reverse count 0000->FFFF->FFFE->etc.)
  *
  * Timers used to capture encoder are assumed never to be 0x8000 and in fact
  * the maximum count of any timer should be 0+-4096 because that is the rated
  * count for one rotation of the E6B2-CZ3E encoder.
  *
  *@param timers Timers used by a certain wheel to calculate the velocity
  *@post Timers used during computation are reset
 */
_Noreturn void find_velocity_task(void* timers);

/**
  * -------------------------- About VELOCITY_FACTOR ---------------------------
  *
  * Simplifies velocity calculation while maintaining precision
  *
  * To prevent slow floating point computations, we can express the velocity
  * in terms of a multiplication factor. Then, for the final calculation, we
  * only need one multiplication operator and one division--simple!
  *
  * The E6B2-CZ3E encoders are quadrature encoders meaning that each pulse is
  * sampled four times. Our encoders are rated 1024 pulses ber rotation (PPR)
  * so for one full rotation, we instead would count 4096 pulses
  *
  * Based on some preliminary testing, the wheel to motor turn ratio is 30.
  * Although there is no *official* spec for this, 1:30 is common for wheelchair
  * DC motors, so we are confident in this estimate.
  *
  * We are also limited in our time measurements as the 16-bit timers on the
  * STM32F103RB are limited to 16 bits or a total count of 2^16-1 = 65535
  *
  * Speed information to the user is limited to 0.1km/h and with some math
  * we find that each tick of TIM1/TIM4 needs to be > 1.83us, any shorter and
  * the counter would roll over before the Z-phase goes high
  *
  * Encoder count should be reset after one full rotation using the Z-phase
  * so that encoder_count/PPR <= 1 we do this to get a finer precision for
  * measurements by limiting the total number of digits used
  *
  * PPR = 4096
  * Gear Ratio = 30
  * Wheel radius = 15.875cm (6.25")
  * Timer prescaler = 40 -> 20MHz/40 = 500kHz or 2us timer period
  *
  * angular_position    = [(encoder_count/PPR) * 2pi] / Gear Ratio
  *                     = 5.11327e-5 * encoder_count in radians
  *
  * linear_distance     = angular_position * wheel_radius
  *                     = 8.11732e-6 * encoder_count in meters
  *
  * velocity            = linear_distance / (timer_count * timer_period)
  *                     = 4.50865 * encoder_count / timer_count in m/s
  *                     = 14.6112 * encoder_count / timer_count in km/h
  *
  * We can then scale the constant to remove any floating point decimals and
  * just use a scaled integer instead. After scaling by 10^4 we are left with
  * the final value stored as an integer (e.g. 3.2187km/h is stored as 32187)
 */

#endif
