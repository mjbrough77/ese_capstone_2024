/**
  *@file i2c_ese.h
  *@author Mitchell Brough
  *@brief Library related to the I2C bus and attached peripherals
  *@version 1.0
  *@date 2024-03-04
  *
  *@copyright Copyright (c)  Mitchell Brough
  *
 */

#ifndef I2C_ESE
#define I2C_ESE

#include "../../project_types.h"

/**
  *@brief Array storing MPU6050 register addresses and write values.
  *
  * On intitailization, `mpu_init` will be stepped through to reset the MPU.
  * Each row represents a seperate write instruction.
 */
extern const uint8_t mpu_init[MPU_RESET_STEPS][MPU_SINGLE_WRITE];

/**************************************************************************
 * Configuration Function Declarations
**************************************************************************/
void configure_i2c2(void);
void configure_i2c2_dma(void);
void enable_mpu_int_pin(void);

/**************************************************************************
 * Peripheral Task Declarations
**************************************************************************/
_Noreturn void mpu_reset_task(void*);
_Noreturn void find_rotation_task(void*);
_Noreturn void eeprom_write_task(void*);
_Noreturn void mpu_read_task(void*);

#endif
