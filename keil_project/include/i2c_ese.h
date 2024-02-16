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

void configure_i2c2(void);		// Sets up I2C2 as controller
void i2c2_waitForReady(void);	// Wait for bus to be free
void i2c2_sendStart(void);		// Send start condition
void i2c2_sendStop(void);			// Send stop condition
void i2c2_startRCWL(void);		// Send 0x1 to address 0x57 on RCWL
uint32_t i2c2_readRCWL(void);	// Read range data from RCWL

void startDistanceMeasure(void); // Full write process to RCWL
uint32_t getDistance(void);			 // Full read process from RCWL

#endif
