/**************************************** 
Manual Drive IO
ENEL 417

Name: Emily Schwab
SID: 200426400
Date Created: 2024-01-19
****************************************/

#include "stm32f10x.h"
#include "utilities.h"
#include "drive.h"

int main(void)
{
	
	/********** System initialization **********/
	clkInit(24);				// 24KHz Clock
	ioInit();					  // Input/Output Pins
	adcInit();				 	// Analog to Digital Converter
	timInit();				  // Timers for PWM Output
	enableTimer3IRQ();	// Timer 3 interrupt enable
	
	stop();
	
	while(1) {
		// No code needed here
	}
	
}
