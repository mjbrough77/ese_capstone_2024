/**************************************** 
Capstone Drive
ENEL 417

Name: Emily Schwab
SID: 200426400
Date Created: 2024-01-19
****************************************/

#include "drive.h"
#include "stm32f10x.h"
#include "utilities.h"


void display(int digits)
{

	// Error check? - value of digits between 0 and 99
	
	bcdOne(digits%10);		// Display ones digit
	bcdTen(digits/10);		// Display tens digit

}

void bcdOne(int num)
{
	// Error check? - value of num between 0 and 9
	
	GPIOA->ODR |= GPIO_ODR_ODR5;		// Enable both digits (BL high)
	
	// Switch case to display
	switch(num)
	{
		case 0:
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR5;		// D0-1 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR4;		// D1-1 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR10;		// D2-1 LOW
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR9;		// D3-1 LOW
			break;
		case 1:
			GPIOB->ODR |= GPIO_ODR_ODR5;		// D0-1 HIGH
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR4;		// D1-1 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR10;		// D2-1 LOW
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR9;		// D3-1 LOW
			break;
		case 2:
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR5;		// D0-1 LOW
			GPIOB->ODR |= GPIO_ODR_ODR4;		// D1-1 HIGH
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR10;		// D2-1 LOW
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR9;		// D3-1 LOW
			break;
		case 3:
			GPIOB->ODR |= GPIO_ODR_ODR5;		// D0-1 HIGH
			GPIOB->ODR |= GPIO_ODR_ODR4;		// D1-1 HIGH
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR10;		// D2-1 LOW
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR9;		// D3-1 LOW
			break;
		case 4:
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR5;		// D0-1 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR4;		// D1-1 LOW
			GPIOB->ODR |= GPIO_ODR_ODR10;		// D2-1 HIGH
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR9;		// D3-1 LOW
			break;
		case 5:
			GPIOB->ODR |= GPIO_ODR_ODR5;		// D0-1 HIGH
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR4;		// D1-1 LOW
			GPIOB->ODR |= GPIO_ODR_ODR10;		// D2-1 HIGH
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR9;		// D3-1 LOW
			break;
		case 6:
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR5;		// D0-1 LOW
			GPIOB->ODR |= GPIO_ODR_ODR4;		// D1-1 HIGH
			GPIOB->ODR |= GPIO_ODR_ODR10;		// D2-1 HIGH
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR9;		// D3-1 LOW
			break;
		case 7:
			GPIOB->ODR |= GPIO_ODR_ODR5;		// D0-1 HIGH
			GPIOB->ODR |= GPIO_ODR_ODR4;		// D1-1 HIGH
			GPIOB->ODR |= GPIO_ODR_ODR10;		// D2-1 HIGH
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR9;		// D3-1 LOW
			break;
		case 8:
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR5;		// D0-1 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR4;		// D1-1 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR10;		// D2-1 LOW
			GPIOA->ODR |= GPIO_ODR_ODR9;		// D3-1 HIGH
			break;
		case 9:
		default:
			GPIOB->ODR |= GPIO_ODR_ODR5;		// D0-1 HIGH
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR4;		// D1-1 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR10;		// D2-1 LOW
			GPIOA->ODR |= GPIO_ODR_ODR9;		// D3-1 HIGH
			break;
	}
}


void bcdTen(int num)
{
	// Error check? - value of num between 0 and 9
	
	GPIOA->ODR |= GPIO_ODR_ODR5;		// Enable both digits (BL high)
	
	// Switch case to display
	switch(num)
	{
		case 0:
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR4;	// D0-2 LOW
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR10;	// D1-2 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR9;	// D2-2 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR8;	// D3-2 LOW
			break;
		case 1:
			GPIOA->ODR |= GPIO_ODR_ODR4;		// D0-2 HIGH
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR10;	// D1-2 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR9;	// D2-2 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR8;	// D3-2 LOW
			break;
		case 2:
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR4;	// D0-2 LOW
			GPIOA->ODR |= GPIO_ODR_ODR10;		// D1-2 HIGH
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR9;	// D2-2 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR8;	// D3-2 LOW
			break;
		case 3:
			GPIOA->ODR |= GPIO_ODR_ODR4;		// D0-2 HIGH
			GPIOA->ODR |= GPIO_ODR_ODR10;		// D1-2 HIGH
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR9;	// D2-2 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR8;	// D3-2 LOW
			break;
		case 4:
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR4;	// D0-2 LOW
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR10;	// D1-2 LOW
			GPIOB->ODR |= GPIO_ODR_ODR9;		// D2-2 HIGH
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR8;	// D3-2 LOW
			break;
		case 5:
			GPIOA->ODR |= GPIO_ODR_ODR4;		// D0-2 HIGH
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR10;	// D1-2 LOW
			GPIOB->ODR |= GPIO_ODR_ODR9;		// D2-2 HIGH
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR8;	// D3-2 LOW
			break;
		case 6:
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR4;	// D0-2 LOW
			GPIOA->ODR |= GPIO_ODR_ODR10;		// D1-2 HIGH
			GPIOB->ODR |= GPIO_ODR_ODR9;		// D2-2 HIGH
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR8;	// D3-2 LOW
			break;
		case 7:
			GPIOA->ODR |= GPIO_ODR_ODR4;		// D0-2 HIGH
			GPIOA->ODR |= GPIO_ODR_ODR10;		// D1-2 HIGH
			GPIOB->ODR |= GPIO_ODR_ODR9;		// D2-2 HIGH
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR8;	// D3-2 LOW
			break;
		case 8:
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR4;	// D0-2 LOW
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR10;	// D1-2 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR9;	// D2-2 LOW
			GPIOB->ODR |= GPIO_ODR_ODR8;		// D3-2 HIGH
			break;
		case 9:
		default:
			GPIOA->ODR |= GPIO_ODR_ODR4;		// D0-2 HIGH
			GPIOA->ODR &= (uint32_t) ~GPIO_ODR_ODR10;	// D1-2 LOW
			GPIOB->ODR &= (uint32_t) ~GPIO_ODR_ODR9;	// D2-2 LOW
			GPIOB->ODR |= GPIO_ODR_ODR8;		// D3-2 HIGH
			break;
	}
}

int manual(void)
{
	
	if ((GPIOC->IDR & GPIO_IDR_IDR7) == 0)
		return 0;		// If input low, return 0
	else
		return 1;		// If input high, return 1

}

void stop(void)
{
	TIM3->EGR |= TIM_EGR_UG; // Reinitialize the counter
	TIM3->ARR = 200; // 200 counts = 20 ms or 50 Hz
	TIM3->CCR1 = 15;
	TIM3->CCR2 = 15;
	TIM3->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; // Enable Timer 3
}

