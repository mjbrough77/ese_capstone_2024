/**
  *@file adc_ese.h
  *@author Mitchell Brough
  *@brief Task and function prototypes for ADC and ADC related devices
  *
  * Contains configuration prototype for ADC1 to enable reading of the weight
  * sensor and a task prototype to conversion of weight sensor input to a
  * numerical weight
  *
  *@version 1.0
  *@date 2024-03-30
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#ifndef ADC_ESE_H
#define ADC_ESE_H

/**
  *@brief Enables ADC1 and starts continuous conversion of PC0.
  *
  * This function should always be called before the scheduler starts.
  *
  *@pre ADC1 clock should be enabled
  *@pre AHB clock is 20MHz
 */
void configure_adc1(void);

/**
  *@brief Converts ADC1 data register reading into a numerical weight
  *
  * If the weight reading is exceeded, this function will notify
  * `eeprom_write_task()` and `error_control_task()` which take the appropriate
  * action
  *
  * Like all tasks monitoring sensor data, notifications should only be sent
  * during state transitions
  *
  *@param param not used
  *@pre `configure_adc1()` has been ran
  *@pre `eeprom_write_task() is created with handle `eeprom_write_handle`
  *@pre `error_control_task()` is created with handle `system_error_handle`
 */
_Noreturn void find_weight_task(void* param);

#endif
