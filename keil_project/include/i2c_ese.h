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
 * I2C Transmission Length Definitions
**************************************************************************/
#define EEPROM_WRITE  sizeof(LogData_t) /* # of bytes sent on log write */
#define MPU_RESET_STEPS            10   /* # of operations to reset the MPU */
#define MPU_SINGLE_WRITE            2   /* Bytes sent during write to MPU */
#define MPU_FIFO_READ               6   /* # of bytes read from FIFO */
#define MPU_READ_ADDRS              3   /* Addresses needed for MPU read */

typedef uint32_t WheelSpeed_t;
typedef uint16_t GyroRead_t;
typedef uint16_t UltrasonicRead_t;
typedef uint8_t Weight_t;
typedef uint8_t* MPUBuffer_t;

typedef struct{
    uint8_t address_high;
    uint8_t address_low;
    Weight_t weight_measure;
    UltrasonicRead_t ultrasonic_left;
    UltrasonicRead_t ultrasonic_right;
    GyroRead_t gyro_x_axis;
    GyroRead_t gyro_y_axis;
    GyroRead_t gyro_z_axis;
    WheelSpeed_t left_wheel_speed;
    WheelSpeed_t right_wheel_speed;
    uint8_t event_flags;
}LogData_t;

/* Stepped through array for resetting MPU6050 */
extern const uint8_t mpu_init[MPU_RESET_STEPS][MPU_SINGLE_WRITE];


extern uint8_t global_mpu_data[MPU_FIFO_READ];

void configure_i2c2(void);
void configure_i2c2_dma(void);
void update_log_dma(LogData_t*);

#endif
