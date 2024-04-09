/**
  *@file adc_ese.h
  *@author Mitchell Brough
  *@brief Task and function prototypes for ADC and ADC related devices
  *@version 1.0
  *@date 2024-04-09
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#ifndef ADC_ESE_H
#define ADC_ESE_H

/**
  *@brief Starts and calibrates ADC1 for reading of the joystick
  *
  *@post ADC1 conversions have NOT been started
 */
void configure_adc1(void);

/**
  *@brief Starts ADC conversions of the joystick outputs
  *
  * Enables the TIM3 (motor driver signals) interrupts because the joystick
  * analog values are sampled before the next period of the motor dirver signals
  *
  * This function assumes ADC1 has already been enabled. ADC1_CR2_ADON must
  * be set twice: once to turn on, another to start conversions
  *
  * See RM0008.pdf
 */
void start_joystick_read(void);

/**
  *@brief Turns off ADC1 and stops TIM3 interrupts
  *
  * Should
  *
 */
void stop_joystick_read(void);

/**
  *@brief Gets the most recent ADC value for the joystick x-axis
  *
  *@return uint32_t the value of the JDR1 register
 */
uint32_t read_joystick_x(void);

/**
  *@brief Gets the most recent ADC value for the joystick y-axis
  *
  *@return uint32_t the value of the JDR2 register
 */
uint32_t read_joystick_y(void);

#endif
