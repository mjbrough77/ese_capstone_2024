/**
  *@file drive.c
  *@author Emily Schwab
  *@brief
  *
  *@version 1.0
  *@date 2024-01-19
  *
  *@copyright Copyright (c) 2024 Emily Schwab
  *
 */

#include "../include/display_ese.h"

/**
  *@brief Writes a value from 0-9 on the ones digit
  *
  * See schematic for pin information
  *
  *@param num value from 0-9
  *@pre num has been type checked by send_to_display()
 */
void write_ones_sevenseg(uint8_t num){
	switch(num){
		case 0:
			GPIOB->BSRR |= GPIO_BSRR_BR5 | GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 1:
			GPIOB->BSRR |= GPIO_BSRR_BS5 | GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 2:
			GPIOB->BSRR |= GPIO_BSRR_BR5 | GPIO_BSRR_BS4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 3:
			GPIOB->BSRR |= GPIO_BSRR_BS5 | GPIO_BSRR_BS4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 4:
			GPIOB->BSRR |= GPIO_BSRR_BR5 | GPIO_BSRR_BR4 | GPIO_BSRR_BS10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 5:
			GPIOB->BSRR |= GPIO_BSRR_BS5 | GPIO_BSRR_BR4 | GPIO_BSRR_BS10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 6:
			GPIOB->BSRR |= GPIO_BSRR_BR5 | GPIO_BSRR_BS4 | GPIO_BSRR_BS10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 7:
			GPIOB->BSRR |= GPIO_BSRR_BS5 | GPIO_BSRR_BS4 | GPIO_BSRR_BS10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 8:
			GPIOB->BSRR |= GPIO_BSRR_BR5 | GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BS9;
			break;
		case 9:
		default:
			GPIOB->BSRR |= GPIO_BSRR_BS5 | GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BS9;
			break;
	}
}

/**
  *@brief Writes a value from 0-9 on the tens digit
  *
  * See schematic for pin information
  *
  *@param num value from 0-9
  *@pre num has been type checked by send_to_display()
 */
void write_tens_sevenseg(uint8_t num){
	switch(num){
		case 0:
            GPIOA->BSRR |= GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BR8;
            break;
		case 1:
            GPIOA->BSRR |= GPIO_BSRR_BS4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BR8;
			break;
		case 2:
            GPIOA->BSRR |= GPIO_BSRR_BR4 | GPIO_BSRR_BS10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BR8;
			break;
		case 3:
            GPIOA->BSRR |= GPIO_BSRR_BS4 | GPIO_BSRR_BS10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BR8;
			break;
		case 4:
            GPIOA->BSRR |= GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BS9 | GPIO_BSRR_BR8;
			break;
		case 5:
            GPIOA->BSRR |= GPIO_BSRR_BS4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BS9 | GPIO_BSRR_BR8;
			break;
		case 6:
            GPIOA->BSRR |= GPIO_BSRR_BR4 | GPIO_BSRR_BS10;
            GPIOB->BSRR |= GPIO_BSRR_BS9 | GPIO_BSRR_BR8;
			break;
		case 7:
            GPIOA->BSRR |= GPIO_BSRR_BS4 | GPIO_BSRR_BS10;
            GPIOB->BSRR |= GPIO_BSRR_BS9 | GPIO_BSRR_BR8;
			break;
		case 8:
            GPIOA->BSRR |= GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BS8;
			break;
		case 9:
		default:
            GPIOA->BSRR |= GPIO_BSRR_BS4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BS8;
			break;
	}
}

/**
  *@brief Outputs a number to the two seven-segment displays
  *
  *@param digits value from 0-99
  *@pre BL has been set to high
 */
void send_to_display(uint8_t digits){
    if(digits > 99) digits = 99;        /* Saturate digits to 99 */
	write_ones_sevenseg(digits % 10);   /* Display ones digit */
	write_tens_sevenseg(digits / 10);   /* Display tens digit */
}

void turn_on_display(void){
    GPIOA->BSRR |= GPIO_BSRR_BS5;
}

void turn_off_display(void){
    GPIOA->BSRR |= GPIO_BSRR_BR5;
}
