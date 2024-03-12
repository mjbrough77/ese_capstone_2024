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
 * I2C Transmission Length Definitions
**************************************************************************/
#define EEPROM_WRITE  sizeof(LogData_t) /* # of bytes sent on log write */
#define PAGE_SIZE                 128   /* EEPROM page width in bytes */
#define MPU_RESET_STEPS            10   /* # of operations to reset the MPU */
#define MPU_SINGLE_WRITE            2   /* Bytes sent during write to MPU */
#define MPU_FIFO_READ               6   /* # of bytes read from FIFO */
#define MPU_READ_ADDRS              3   /* Addresses needed for MPU read */


/**************************************************************************
 * Typedefs and structures
**************************************************************************/
typedef uint16_t WheelSpeed_t;
typedef uint16_t GyroRead_t;
typedef uint16_t UltrasonicRead_t;
typedef uint8_t Weight_t;

/**
  *@brief Stucture used to store measured values to save to EEPROM log.
  *
  * The `event_flags` member variable indicates when any measured values
  * exceed the tolerated threshold.
  *
  * Bits [2:0] are set if any are exceeded after a system power-on.
  *
  * [2] W = user weight exceeded
  * [1] D = chair has gotten to close to an obstacle
  * [0] T = chair has exceed tilt requirements on any axis
 */
typedef struct{
    uint8_t address_high;               /* EEPROM high address for page write */
    uint8_t address_low;                /* EEPROM low address for page write */
    uint8_t event_flags;                /* xxxx xWDT */
    Weight_t weight_measure;            /* ADC value from weight sensor */
    UltrasonicRead_t ultrasonic_left;   /* Distance measure from left US */
    UltrasonicRead_t ultrasonic_right;  /* Distance measure from right US */
    GyroRead_t gyro_x_axis;             /* MPU6050 x-axis angular speed */
    GyroRead_t gyro_y_axis;             /* MPU6050 y-axis angular speed */
    GyroRead_t gyro_z_axis;             /* MPU6050 z-axis angular speed */
    WheelSpeed_t left_wheel_speed;      /* Velocity in 100um/s */
    WheelSpeed_t right_wheel_speed;     /* Velocity in 100um/s */
}LogData_t;

typedef struct{
    GyroRead_t gyro_x_axis;             /* MPU6050 x-axis angular speed */
    GyroRead_t gyro_y_axis;             /* MPU6050 y-axis angular speed */
    GyroRead_t gyro_z_axis;             /* MPU6050 z-axis angular speed */
}MPUData_t;

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
 * Function Declarations
**************************************************************************/
void configure_i2c2(void);
void configure_i2c2_dma(void);
void update_log_dma(LogData_t*);

#endif
