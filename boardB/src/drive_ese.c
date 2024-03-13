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
 
#include "stm32f10x.h"

#include "../include/drive.h"
#include "../include/utilities.h"

void display(uint8_t digits)
{
    if(digits > 99) digits = 99;    /* Saturate digits to 99 */
	bcdOne(digits % 10);            /* Display ones digit */
	bcdTen(digits / 10);            /* Display tens digit */
}

void bcdOne(uint8_t num){
	
	/* Switch case to display */
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

void bcdTen(uint8_t num){
	// Switch case to display
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

uint8_t manual(void){
	
	if ((GPIOC->IDR & GPIO_IDR_IDR7) == 0)
		return 0;		// If input low, return 0
	else
		return 1;		// If input high, return 1

}

void stop(void){
	TIM3->EGR |= TIM_EGR_UG; // Reinitialize the counter
	TIM3->ARR = 200; // 200 counts = 20 ms or 50 Hz
	TIM3->CCR1 = 15;
	TIM3->CCR2 = 15;
	TIM3->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; // Enable Timer 3
}

