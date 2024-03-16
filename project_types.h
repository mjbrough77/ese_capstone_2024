#include "stdint.h"

#define ULTRASONIC_COUNT 2              /* Number of ultrasonics */
#define HALF_SPEED_OF_SOUND 140         /* m/s */
#define ULTRASONIC_LEFT_OFFSET  474     /* us, found during testing */
#define ULTRASONIC_RIGHT_OFFSET 460     /* us, found during testing */

/**************************************************************************
 * Typedefs and structures
**************************************************************************/
typedef uint16_t WheelSpeed_t;      /* Wheel speed always < 3.2187 km/h */
typedef uint16_t Gyro_t;            /* Gyroscope is 2 bytes per axis */
typedef uint32_t Ultrasonic_t;      /* Ultrasonics use 4 bytes */
typedef uint16_t PageNum_t;         /* Size based on TOTAL_PAGES */
typedef uint8_t Weight_t;           /* Only using 8 bits of 12 bit ADC */

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
    Ultrasonic_t ultrasonic_left;       /* Distance measure from left US */
    Ultrasonic_t ultrasonic_right;      /* Distance measure from right US */
    Gyro_t gyro_x_axis;                 /* MPU6050 x-axis angular speed */
    Gyro_t gyro_y_axis;                 /* MPU6050 y-axis angular speed */
    Gyro_t gyro_z_axis;                 /* MPU6050 z-axis angular speed */
    WheelSpeed_t left_wheel_speed;      /* Velocity in 100cm/s */
    WheelSpeed_t right_wheel_speed;     /* Velocity in 100cm/s */
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
    Gyro_t gyro_x_axis;             /* MPU6050 x-axis angular speed */
    Gyro_t gyro_y_axis;             /* MPU6050 y-axis angular speed */
    Gyro_t gyro_z_axis;             /* MPU6050 z-axis angular speed */
}MPUData_t;
