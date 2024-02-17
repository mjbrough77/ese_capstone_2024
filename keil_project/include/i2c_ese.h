/*
 * FILENAME: i2c_lib.h
 *
 * DESCRIPTION: Header file for i2c_lib.c
 *
 * AUTHOR: Mitchell Brough, 200239781
*/

#ifndef I2C_ESE
#define I2C_ESE

void configure_i2c2(void);
void I2C2_EV_IRQHandler(void);
void I2C2_ER_IRQHandler(void);

#endif
