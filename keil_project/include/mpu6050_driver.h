#ifndef MPU6050_DRIVER_H
#define MPU6050_DRIVER_H

#include "stm32f10x.h"

#define REG_SMPLRT_DIV          0x19
#define REG_CONFIG              0x1A
#define REG_ACCEL_CONFIG        0x1C
#define REG_FIFO_EN             0x23
#define REG_INT_ENABLE          0x38
#define REG_SIGNAL_PATH_RESET   0x68
#define REG_USER_CTRL           0x6A
#define REG_PWR_MGMT_1          0x6B

typedef struct{
    uint8_t reg_value;      // An MPU register
    uint8_t init_value;     // The register value used to initilaize
} RegReset;

void configure_mpu6050(void);

#endif
