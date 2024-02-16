/*
 * FILENAME: i2c_lib.c
 *
 * DESCRIPTION: Library concerned with I2C and RCWL-1601 communication
 *	Note: Most of this file is based off source code from nicerland.com
 *	and the lab
 *
 * AUTHOR: Mitchell Brough, 200239781
*/

#include "../include/i2c_ese.h"


/************* Configuration and I2C transfer sequence functions *************/	
void configure_i2c2(void){
	I2C2->CR2 |= 0x18; // 0x18 = 24MHz
	I2C2->CCR = 0x78;  // 100kHz 50% DC = 5us high/low = 120 cycles @ 24MHz
	I2C2->TRISE = 25;  // 1000ns / 41.67us = 24 + 1 = 25
}

void i2c2_waitForReady(void){
	while(I2C2->SR2 & I2C_SR2_BUSY); // Wait for line to be free
}

void i2c2_sendStart(void){
	I2C2->CR1 |= I2C_CR1_START;         // Generate start
	while( !(I2C2->SR1 & I2C_SR1_SB) ); // Wait for start generation
	I2C2->SR2;							// Read SR2
}

void i2c2_sendStop(void){
	I2C2->CR1 |= I2C_CR1_STOP;		// Generate stop
	while(I2C2->SR2 & I2C_SR2_MSL);	// Wait for transition to peripheral
}

void i2c2_startRCWL(void){
	I2C2->DR = ((0x57) << 1); 			  // Address 0x57 with write
	while( !(I2C2->SR1 & I2C_SR1_ADDR) ); // Wait for address send
	I2C2->SR2;							  // Read SR2
	
	I2C2->DR = 0x1;	// Send 0x1 to start ranging session

	while( !(I2C2->SR1 & I2C_SR1_TXE) ); // Wait for empty DR
}

uint32_t i2c2_readRCWL(void){
	uint32_t distance;
	
	I2C2->DR = ((0x57) << 1) + 1; 		  // Address 0x57 with read
	while( !(I2C2->SR1 & I2C_SR1_ADDR) ); // Wait for address send
	I2C2->SR2;							  // Read SR2

	I2C2->CR1 |= I2C_CR1_ACK;			  // Send ACK
	while( !(I2C2->SR1 & I2C_SR1_BTF) );  // Wait for full buffer
	I2C2->CR1 &= ~I2C_CR1_ACK;			  // Send NACK
	
	distance = (uint32_t)(I2C2->DR) << 16; // Read first byte
	
	i2c2_sendStop();
	
	distance += (uint32_t)(I2C2->DR) << 8; // Read second byte
	
	while( !(I2C2->SR1 & I2C_SR1_RXNE) );  // Wait for empty reciever
	
	distance += I2C2->DR; // Read third byte
	
	return distance; // Return distance in um from RCWL
}

/************** External functions used by SysTick, r/w process to RCWL **************/	
void startDistanceMeasure(void){
	I2C2->CR1 |= I2C_CR1_PE;
	
	i2c2_waitForReady();	// Refer to function definitions
	i2c2_sendStart();
	i2c2_startRCWL();
	i2c2_sendStop();
	
	I2C2->CR1 &= ~I2C_CR1_PE;	// Found I needed to disable peripheral for proper continuous r/w sessions
}

uint32_t getDistance(void){
	uint32_t distance;
	
	I2C2->CR1 |= I2C_CR1_PE;
	
	i2c2_waitForReady();				// Refer to function definitions
	i2c2_sendStart();
	distance = i2c2_readRCWL();

	I2C2->CR1 &= ~I2C_CR1_PE;	
	
	return distance;	// Distance is in um
}
