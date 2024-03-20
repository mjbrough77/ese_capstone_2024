#ifndef PROJECT_TYPES_H
#define PROJECT_TYPES_H

#include "stdint.h"

/**************************************************************************
 * Macros for debugging [Define to turn on]
**************************************************************************/
#define MPU_RESET_SKIP
#define EEPROM_TASK_SUSPEND
#define MPU_TASK_SUSPEND
//#define SEND_SPEED_TASK_SUSPEND
//#define SEND_ULTRASONIC_TASK_SUSPEND

/**************************************************************************
 * Ultrasonic Hardware Definitions
**************************************************************************/
#define HALF_SPEED_OF_SOUND     140     /* [m/s] */
#define ULTRASONIC_RIGHT_OFFSET 460     /* [us], found during testing */
#define ULTRASONIC_LEFT_OFFSET  474     /* [us], found during testing */

/**************************************************************************
 * Wheel Definitions
**************************************************************************/
#define SPEED_SCALE             1000    /* Speed data stored as [10^4 km/h] */
#define VELOCITY_FACTOR         146112  /* See boardA/src/timers_ese.c */
#define WHEEL_COUNT             2
#define MAX_SPEED               35000   /* [10^4 km/h] */

/**************************************************************************
 * USART Flag Values
**************************************************************************/
#define USART_READY             0xFF

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
#define PAGE_SIZE               128   /* ROM page width in bytes */
#define PAGES_PER_BLOCK         200   /* Memory divided into blocks */
#define TOTAL_PAGES             400   /* Total number of EEPROM pages */

/**************************************************************************
 * I2C Transmission Length Definitions
**************************************************************************/
#define MPU_RESET_STEPS         10   /* # of operations to reset the MPU */
#define MPU_SINGLE_WRITE        2   /* Bytes sent during write to MPU */
#define MPU_FIFO_READ           6   /* # of bytes read from FIFO */
#define MPU_READ_ADDRS          3   /* Addresses needed for MPU read */

/**************************************************************************
 * Typedefs and structures
**************************************************************************/
typedef int32_t  WheelVelocity_t;   /* Must record speeds > MAX_SPEED */
typedef uint16_t ChairSpeed_t;      /* Must record speeds > MAX_SPEED */
typedef uint16_t Gyro_t;            /* Gyroscope is 2 bytes per axis */
typedef uint32_t Ultrasonic_t;      /* Ultrasonics use 4 bytes */
typedef uint16_t PageNum_t;         /* Size based on TOTAL_PAGES */
typedef uint8_t  Weight_t;          /* Only using 8 bits of 12 bit ADC */

/**
  *@brief Stucture used to store measured values to save to EEPROM log.
  *
  * The `event_flags` member variable indicates when any measured values
  * exceed the tolerated threshold.
  *
  * Bits [3:0] are set if any are exceeded after a system power-on.
  *
  * [3] S = speed exceeded
  * [2] W = user weight exceeded
  * [1] D = chair has gotten to close to an obstacle
  * [0] T = chair has exceed tilt requirements on any axis
 */
typedef struct{
    uint8_t address_high;               /* EEPROM high address for page write */
    uint8_t address_low;                /* EEPROM low address for page write */
    uint8_t event_flags;                /* xxxx SWDT */
    Weight_t weight_measure;            /* ADC value from weight sensor */
    Ultrasonic_t ultrasonic_left;       /* Distance measure from left [um] */
    Ultrasonic_t ultrasonic_right;      /* Distance measure from right [um] */
    WheelVelocity_t left_wheel_speed;   /* Velocity [10^4 km/h] */
    WheelVelocity_t right_wheel_speed;  /* Velocity [10^4 km/h] */
    Gyro_t gyro_x_axis;                 /* MPU6050 x-axis angular speed */
    Gyro_t gyro_y_axis;                 /* MPU6050 y-axis angular speed */
    Gyro_t gyro_z_axis;                 /* MPU6050 z-axis angular speed */
}LogData_t;

/**
  *@brief Passes data between MPU buffer in memory to `eeprom_write_task`
  *
  * To ensure reetrancy, the DMA controller and `eeprom_write_task` should not
  * have concurrent access to the MPU buffer in memory, to get around this, the
  * data is copied into this structure before being passed via queue to
  * `eeprom_write_task`
 */
typedef struct{
    Gyro_t gyro_x_axis;     /* MPU6050 x-axis angular speed */
    Gyro_t gyro_y_axis;     /* MPU6050 y-axis angular speed */
    Gyro_t gyro_z_axis;     /* MPU6050 z-axis angular speed */
}MPUData_t;

typedef struct{
    Ultrasonic_t left_data;
    Ultrasonic_t right_data;
}Distances_t;

typedef struct{
    WheelVelocity_t left_speed;
    WheelVelocity_t right_speed;
}Speeds_t;

#endif
