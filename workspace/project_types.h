#ifndef PROJECT_TYPES_H
#define PROJECT_TYPES_H

#include "FreeRTOS.h"
#include "queue.h"

/**************************************************************************
 * Macros for debugging [Define to turn on]
**************************************************************************/
//#define MPU_RESET_SKIP
#define EEPROM_TASK_SUSPEND
//#define MPU_TASK_SUSPEND
//#define SEND_SPEED_TASK_SUSPEND
//#define SEND_ULTRASONIC_TASK_SUSPEND

/**************************************************************************
 * Hardware limits
**************************************************************************/
#define MAX_WEIGHT              250     /* [lbs] */
#define MAX_SPEED               35000   /* [10^4 km/h] */
#define MAX_TILT                15.0f   /* Degrees */

/**************************************************************************
 * Ultrasonic Hardware Definitions
**************************************************************************/
#define HALF_SPEED_OF_SOUND     140     /* [m/s] */
#define ULTRASONIC_RIGHT_OFFSET 460     /* [us], found during testing */
#define ULTRASONIC_LEFT_OFFSET  474     /* [us], found during testing */

/**************************************************************************
 * MPU6050 Hardware Definitions
**************************************************************************/
#define MPU_SAMPLE_TIME         8E-03f  /* [s], programmed sample rate */
#define GYRO_SENSITIVITY        65.5f   /* From datasheet */
#define ACCEL_SENSITIVITY       8192.0f /* From datasheet */
#define GYRO_X_OFFSET           -1.49557137f
#define GYRO_Y_OFFSET           -0.3351143f
#define GYRO_Z_OFFSET           0.459440142f
#define ACCEL_X_OFFSET          -88.1409912f
#define ACCEL_Y_OFFSET          -0.196265712f

/**************************************************************************
 * Wheel Definitions
**************************************************************************/
#define SPEED_SCALE             1000    /* Speed data stored as 10^4 km/h */
#define VELOCITY_FACTOR         146112  /* See boardA/src/timers_ese.c */
#define SPEED_SAMPLE_MS         15      /* [ms], based on max z-phase period */

/**************************************************************************
 * USART Flag Values
**************************************************************************/
#define USART_READY             0x52
#define USART_SYS_FAIL          0xFFFF  /* Chair speed always < MAX_SPEED */
#define USART_STOP_CHAIR        0xFFFE  /* Chair speed always < MAX_SPEED */

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
 * EEPROM Hardware Definitions
**************************************************************************/
#define PAGE_SIZE               128   /* ROM page width in bytes */
#define PAGES_PER_BLOCK         200   /* Memory divided into blocks */
#define TOTAL_PAGES             400   /* Total number of EEPROM pages */

/**************************************************************************
 * I2C Transmission Length Definitions
**************************************************************************/
#define MPU_RESET_STEPS         11  /* # of operations to reset the MPU */
#define MPU_SINGLE_WRITE        2   /* Bytes sent during write to MPU */
#define MPU_FIFO_READ           12  /* # of bytes read from FIFO */
#define MPU_READ_ADDRS          3   /* Addresses needed for MPU read */

/**************************************************************************
 * Misc. Useful Constants
**************************************************************************/
#define PI                      3.14159265f

/**************************************************************************
 * Typedefs and structures
**************************************************************************/
typedef int32_t  WheelVelocity_t;   /* Must record speeds > MAX_SPEED */
typedef uint16_t ChairSpeed_t;      /* Must record speeds > MAX_SPEED */
typedef int16_t Gyro_t;             /* Gyroscope is 2 bytes per axis */
typedef int16_t Accel_t;            /* Accel is 2 bytes per axis */
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
    Accel_t accel_x_axis;               /* MPU6050 x-axis acceleration */
    Accel_t accel_y_axis;               /* MPU6050 y-axis acceleration */
    Accel_t accel_z_axis;               /* MPU6050 z-axis acceleration */
    Gyro_t gyro_x_axis;                 /* MPU6050 x-axis angular speed */
    Gyro_t gyro_y_axis;                 /* MPU6050 y-axis angular speed */
    Gyro_t gyro_z_axis;                 /* MPU6050 z-axis angular speed */
}LogData_t;

/**
  *@brief Container for FIFO data from the MPU6050
  *
 */
typedef struct{
    Accel_t accel_x_axis;   /* MPU6050 x-axis raw acceleration data */
    Accel_t accel_y_axis;   /* MPU6050 y-axis raw acceleration data */
    Accel_t accel_z_axis;   /* MPU6050 z-axis raw acceleration data */
    Gyro_t gyro_x_axis;     /* MPU6050 x-axis raw angular speed data */
    Gyro_t gyro_y_axis;     /* MPU6050 y-axis raw angular speed data */
    Gyro_t gyro_z_axis;     /* MPU6050 z-axis raw angular speed data */
}MPUData_t;

typedef struct{
    Ultrasonic_t left_data;
    Ultrasonic_t right_data;
}Distances_t;

typedef struct{
    WheelVelocity_t left_speed;
    WheelVelocity_t right_speed;
}Speeds_t;

typedef struct{
    TIM_TypeDef* z_phase_timer;
    TIM_TypeDef* encoder_timer;
    QueueHandle_t side;
}EncoderTimers_t;

/**************************************************************************
 * Useful functions
**************************************************************************/
extern float fast_hypotenuse(float, float);

#endif
