/*
 * FILENAME: i2c_lib.h
 *
 * DESCRIPTION: Header file for i2c_lib.c
 *
 * AUTHOR: Mitchell Brough, 200239781
*/

#ifndef I2C_ESE
#define I2C_ESE

#include "stm32f10x.h"

#define ADDR_EEPROM             0x58
#define ADDR_MPU                0x68

#define REG_SMPLRT_DIV          0x19
#define REG_CONFIG              0x1A
#define REG_ACCEL_CONFIG        0x1C
#define REG_FIFO_EN             0x23
#define REG_INT_ENABLE          0x38
#define REG_SIGNAL_PATH_RESET   0x68
#define REG_USER_CTRL           0x6A
#define REG_PWR_MGMT_1          0x6B
#define REG_FIFO                0x74

#define MPU_RESET_STEPS         10   /* # of operations to reset the MPU */
#define MPU_SINGLE_WRITE         2   /* Single byte write sends 2 values */
#define MPU_BURST_READ           6   /* Gyroscope data stored in 6 bytes */
#define MPU_READ_ADDRS           3   /* Number of addresses for one MPU read */

/* Stepped through array for resetting MPU6050 */
extern const uint8_t mpu_init[MPU_RESET_STEPS][MPU_SINGLE_WRITE];
extern uint8_t mpu_data[MPU_BURST_READ];

void configure_i2c2(void);
void configure_i2c2_dma(void);
void reset_i2c2(void);


#endif
