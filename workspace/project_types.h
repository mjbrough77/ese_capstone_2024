/**
  *@file project_types.h
  *@author Mitchell Brough
  *@brief Macros, typedefs used across entire project and both boards
  *
  * Provides a localized file where all macros and typedefs are stored.
  * Any new structures, #defines should *always* go in this file
  *
  *@version 1.0
  *@date 2024-03-31
  *
  *@copyright Copyright (c) 2024
 */

#ifndef PROJECT_TYPES_H
#define PROJECT_TYPES_H

#include "FreeRTOS.h"   /* QueueHandle_t */
#include "queue.h"      /* QueueHandle_t */

/**************************************************************************
 * Macros for debugging [Uncomment to turn on]
**************************************************************************/
//#define MPU_RESET_SKIP
//#define MPU_TASK_SUSPEND
//#define TILT_TASK_SUSPEND
//#define WEIGHT_TASK_SUSPEND
#define EEPROM_TASK_SUSPEND

/**************************************************************************
 * Sample times
**************************************************************************/
#define MPU_SAMPLE_TIME         8e-03f  /* [s], programmed in reset vector */
#define SPEED_SAMPLE_MS         8       /* [ms], based on max z-phase period */
#define WEIGHT_SAMPLE_MS        100     /* [ms] arbitrary */
#define STARTUP_TIME_MS         500     /* [ms] let system 'warm up' */

/**************************************************************************
 * Sensor limits
**************************************************************************/
#define MIN_WEIGHT              40          /* [lbs] */
#define MAX_WEIGHT              220         /* [lbs] */
#define MAX_SPEED               65400       /* [10^4 km/h] */
#define MAX_TILT_ROLL           15.0f       /* [deg] */
#define MAX_TILT_PITCH           9.0f       /* [deg] */
#define SLOW_DISTANCE          304800       /* [um] */
#define STOP_DISTANCE           50000       /* [um] */

/**************************************************************************
 * Ultrasonic Definitions
**************************************************************************/
#define HALF_SPEED_OF_SOUND     140     /* [m/s] */
#define ULTRASONIC_RIGHT_OFFSET 460     /* [us], found during testing */
#define ULTRASONIC_LEFT_OFFSET  474     /* [us], found during testing */

/**************************************************************************
 * Weight Sensor Definitions
**************************************************************************/
#define ADC_RESOLUTION          8.056640625e-4f /* [V/LSB] */
#define VOLTAGE_TARE            1.4962039f      /* [V] */
#define WEIGHT_RESOLUTION       5.0e-4f         /* [V/lb] */

/**************************************************************************
 * Wheel Definitions
**************************************************************************/
#define SPEED_SCALE             1000    /* Speed data stored as 10^4 km/h */
#define VELOCITY_FACTOR         146112  /* See boardA/src/timers_ese.c */

/**************************************************************************
 * MPU6050 Definitions
**************************************************************************/
#define GYRO_SENSITIVITY        65.5f           /* [LSB/deg/s] from datasheet */
#define ACCEL_SENSITIVITY       8192.0f         /* [LSB/g] from datasheet */
#define GYRO_X_OFFSET           -1.59863269f    /* [deg] average error */
#define GYRO_Y_OFFSET           -0.3351143f     /* [deg] average error */
#define GYRO_Z_OFFSET           0.453847766f    /* [deg] average error */
#define ACCEL_X_OFFSET          -78.4758759f    /* [deg] average error */
#define ACCEL_Y_OFFSET          -0.627747715f   /* [deg] average error */

/**************************************************************************
 * EEPROM Hardware Definitions
**************************************************************************/
#define UPDATE_LOG_MS           1000    /* How often the EEPROM is written */
#define PAGE_SIZE               128     /* ROM page width in bytes */
#define PAGES_PER_BLOCK         200     /* Memory divided into blocks */
#define TOTAL_PAGES             400     /* Total number of EEPROM pages */

/**************************************************************************
 * Flag Values over USART (assume chair speed can never be > MAX_SPEED)
**************************************************************************/
#define USART_STOP_CHAIR        0xFFFFFFF1
#define USART_CLEAR_ERROR       0xFFFFFFF0
#define USART_READY             0xFF

/**************************************************************************
 * Task Notification Flags (Maximum of 32--see FreeRTOS reference manual)
**************************************************************************/
#define MAXTILT_NOTIFY              0x1
#define DISTANCE_NOTIFY             0x2
#define WEIGHT_NOTIFY               0x4
#define OVERSPEED_NOTIFY            0x8
#define SLOW_CHAIR_NOTIFY           0x10
#define STOP_CHAIR_NOTIFY           0x20
#define TRIG_PULSE_NOTIFY           0x40
#define I2C2_ERR_NOTIFY             0x80
#define RESUME_SPEED_NOTIFY         0x100
#define ERROR_CNTRL_TILT_NOTIFY     0x200
#define ERROR_CNTRL_WEIGHT_NOTIFY   0x400
#define CLEAR_ERR_NOTIFY            0x80000000

/**************************************************************************
 * Seven-segment display messages
**************************************************************************/
#define DISPLAY_ERROR           88      /* Display this if system error */

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
#define REG_ACCEL_CONFIG        0x1C
#define REG_FIFO_EN             0x23
#define REG_INT_ENABLE          0x38
#define REG_SIGNAL_PATH_RESET   0x68
#define REG_USER_CTRL           0x6A
#define REG_PWR_MGMT_1          0x6B
#define REG_FIFO                0x74

/**************************************************************************
 * I2C Transmission Length Definitions
**************************************************************************/
#define MPU_RESET_STEPS         11  /* # of operations to reset the MPU */
#define MPU_SINGLE_WRITE        2   /* Bytes sent during write to MPU */
#define MPU_FIFO_READ           12  /* # of bytes read from FIFO */
#define MPU_READ_ADDRS          3   /* Addresses needed for MPU read */

/**************************************************************************
 * Miscellaneous Useful Constants
**************************************************************************/
#define PI                      3.14159265f

/**************************************************************************
 * Typedefs and structures
**************************************************************************/
typedef int32_t  WheelVelocity_t;   /* Must record speeds > MAX_SPEED */
typedef uint32_t Ultrasonic_t;      /* Ultrasonics use 4 bytes */
typedef uint32_t UsartBuffer_t;     /* Buffer size for USART data in boardT */
typedef int16_t Gyro_t;             /* Gyroscope is 2 bytes per axis */
typedef int16_t Accel_t;            /* Accel is 2 bytes per axis */
typedef uint16_t ChairSpeed_t;      /* Must record speeds > MAX_SPEED */
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
    Accel_t accel_x_axis;               /* MPU6050 x-axis acceleration */
    Accel_t accel_y_axis;               /* MPU6050 y-axis acceleration */
    Accel_t accel_z_axis;               /* MPU6050 z-axis acceleration */
    Gyro_t gyro_x_axis;                 /* MPU6050 x-axis angular speed */
    Gyro_t gyro_y_axis;                 /* MPU6050 y-axis angular speed */
    Gyro_t gyro_z_axis;                 /* MPU6050 z-axis angular speed */
}LogData_t;

/**
  *@brief Container for FIFO data from the MPU6050
 */
typedef struct{
    Accel_t accel_x_axis;   /* MPU6050 x-axis raw acceleration data */
    Accel_t accel_y_axis;   /* MPU6050 y-axis raw acceleration data */
    Accel_t accel_z_axis;   /* MPU6050 z-axis raw acceleration data */
    Gyro_t gyro_x_axis;     /* MPU6050 x-axis raw angular speed data */
    Gyro_t gyro_y_axis;     /* MPU6050 y-axis raw angular speed data */
    Gyro_t gyro_z_axis;     /* MPU6050 z-axis raw angular speed data */
}MPUData_t;

/**
  *@brief Container for Ultrasonic data from BoardT
 */
typedef struct{
    Ultrasonic_t left_data;     /* [um] measurement from left ultrasonic */
    Ultrasonic_t right_data;    /* [um] measurement from right ultrasonic */
}Distances_t;

/**
  *@brief Parameter for `find_velocity_task()` to distinguish motor encoders
 */
typedef struct{
    TIM_TypeDef* z_phase_timer; /* Timer used for input capture of z-phase */
    TIM_TypeDef* encoder_timer; /* Timer used for a,b-phase capture */
    QueueHandle_t side;         /* Queue holding left/right velocity data */
}EncoderTimers_t;

#endif
