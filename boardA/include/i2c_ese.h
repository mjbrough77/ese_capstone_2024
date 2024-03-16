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

#include "stm32f10x.h"

/**************************************************************************
 * I2C Device Addresses
**************************************************************************/
#define ADDR_EEPROM             0x50
#define ADDR_MPU                0x68


/**************************************************************************
 * MPU6050 Register Addresses
**************************************************************************/
#define REG_SMPLRT_DIV          0x19
#define REG_CONFIG              0x1A
#define REG_GYRO_CONFIG         0x1B
#define REG_FIFO_EN             0x23
#define REG_INT_ENABLE          0x38
#define REG_SIGNAL_PATH_RESET   0x68
#define REG_USER_CTRL           0x6A
#define REG_PWR_MGMT_1          0x6B
#define REG_FIFO                0x74


/**************************************************************************
 * EEPROM Hardware Definitions
**************************************************************************/
#define PAGE_SIZE                 128   /* ROM page width in bytes */
#define PAGES_PER_BLOCK           200   /* Memory divided into blocks */
#define TOTAL_PAGES               400   /* Total number of EEPROM pages */


/**************************************************************************
 * I2C Transmission Length Definitions
**************************************************************************/
#define MPU_RESET_STEPS            10   /* # of operations to reset the MPU */
#define MPU_SINGLE_WRITE            2   /* Bytes sent during write to MPU */
#define MPU_FIFO_READ               6   /* # of bytes read from FIFO */
#define MPU_READ_ADDRS              3   /* Addresses needed for MPU read */


/**************************************************************************
 * MPU6050 Initialization Array
**************************************************************************/
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


/**************************************************************************
 * Peripheral Task Declarations
**************************************************************************/
void mpu_reset_task(void*);
void eeprom_write_task(void*);
void mpu_read_task(void*);

#endif
