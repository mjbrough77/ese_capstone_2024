/**************************************** 
Capstone Drive
ENEL 417

Name: Emily Schwab
SID: 200426400
Date Created: 2024-01-19
****************************************/

#include "stm32f10x.h"

void display(int);	// Display any integer from 0 to 99 on the two digit 7-segment displays
void bcdOne(int);		// Display any integer from 0 to 9 on the 'ones' 7-segment display
void bcdTen(int);		// Display any integer from 0 to 9 on the 'tens' 7-segment display

int manual(void);		// Returns true if manual vs auto (i.e. safety) switch is set to manual, false otherwise

void stop(void);

