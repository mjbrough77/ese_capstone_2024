/**
  *@file utilities.c
  *@author Emily Schwab
  *@brief
  *
  *@version 1.0
  *@date 2024-01-19
  *
  *@copyright Copyright (c) 2024 Emily Schwab
 */
 
#include "stm32f10x.h"

#include "../include/utilities_ese.h"

// Initialize ADC
void adcInit(void)
{
	
	// clocks initialized in clkInit()
	// IO pin initialized in ioInit()
	
	//ADC1->CR2 |= ADC_CR2_CONT;	// Turn on continuous conversion mode (may need to not do this) 
	ADC1->CR2 |= ADC_CR2_ADON; 	// Enable ADC
	
}

// Read ADC
uint32_t adcRead(uint32_t chnl)
{
	
	uint32_t data;
	
	// Clear channel intended to convert
	switch (chnl) {
		case 10:
			ADC1->SQR3 = 0x0000000A;		// next conversion from ADC channel 10
			break;
		case 11:
			ADC1->SQR3 = 0x0000000B;		// next conversion from ADC channel 11
			break;
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 12:
		case 13:
		case 14:
		case 15:
		default:
			ADC1->SQR1 = 0x00000000;
			ADC1->SQR2 = 0x00000000;
			ADC1->SQR3 = 0x00000000;
			break;
	}
	
	ADC1->CR2 |= ADC_CR2_ADON;	// Start the conversion sequence
	
	while ((ADC1->SR & ADC_SR_EOC) == 0);	// Wait until EOC flag is set
	
	data = ADC1->DR;	// Read from data register
	
	return data;	// Return read value
	
}

// Initialize timers
void timInit(void) 
{
	/*
	// Timer 1, 2, and 4 need 60ms duty cycles
	// Timer 2 channel 2 output needs 10us pulse width
	// Timer 1 and 4 need to be configured as inputs and need to watch for the input somehow...idk how
	
	// TIMER 1
	TIM1->CR1 |= TIM_CR1_CEN;	 // Enable Timer 1
	TIM1->EGR |= TIM_EGR_UG;	 // Reinitialize the counter
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // PWM mode 1 (Upcounting)
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1FE; // Preload Enable, Fast Enable
	TIM1->CCER |= TIM_CCER_CC1E;		// Enable CH1 on output pin (PA8) (Header D7)
	//TIM1->CCER |= TIM_CCER_CC3E;	// Enable CH3 on output pin (PA10) (Header D2)
	TIM1->PSC = 0x095F; // Divide 24 MHz by 2400 (PSC+1), PSC_CLK= 10000 Hz, 1 count = 0.1 ms
	TIM1->ARR = 200; // 200 counts = 20 ms or 50 Hz
	TIM1->CCR1 = 5;	// 5 counts = 0.5 ms = 2.5% duty cycle = 0 degree axis
	TIM1->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; // Enable Timer 1
	
	// TIMER 2
	TIM2->CR1 |= TIM_CR1_CEN;	 // Enable Timer 2
	TIM2->EGR |= TIM_EGR_UG;	 // Reinitialize the counter
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // PWM mode 1 (Upcounting)
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1FE; // Preload Enable, Fast Enable
	TIM2->CCER |= TIM_CCER_CC2E; 	// Enable CH2 on output pin (PA1) (Header A1) 
	TIM2->PSC = 0x095F; // Divide 24 MHz by 2400 (PSC+1), PSC_CLK= 10000 Hz, 1 count = 0.1 ms
	TIM2->ARR = 200; // 200 counts = 20 ms or 50 Hz
	TIM2->CCR1 = 5;	// 5 counts = 0.5 ms = 2.5% duty cycle = 0 degree axis
	TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; // Enable Timer 2
	*/
	
	// TIMER 3
	TIM3->CR1 |= TIM_CR1_CEN; // Enable Timer 3
	TIM3->EGR |= TIM_EGR_UG; // Reinitialize the counter
	TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // PWM mode 1 (Upcounting) on CH1
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1FE; // Preload Enable, Fast Enable on CH1
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;	// PWM mode 1 (Upcounting) on CH2
	TIM3->CCMR1 |= TIM_CCMR1_OC2PE | TIM_CCMR1_OC2FE;	// Preload Enable, Fast Enable on CH2
	TIM3->CCER |= TIM_CCER_CC1E; 	// Enable CH1 on output pin PA6
	TIM3->CCER |= TIM_CCER_CC2E;	// Enable CH2 on output pin PA7
	TIM3->PSC = 0x095F; // Divide 24 MHz by 2400 (PSC+1), PSC_CLK= 10000 Hz, 1 count = 0.1 ms
	TIM3->ARR = 200; // 200 counts = 20 ms or 50 Hz
	TIM3->CCR1 = 15;	// 15 counts = 1.5 ms = 7.5% duty cycle
	TIM3->CCR2 = 15;	// 15 counts = 1.5 ms = 7.5% duty cycle
	TIM3->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; // Enable Timer3
	
	/*
	// TIMER 4
	TIM4->CR1 |= TIM_CR1_CEN; // Enable Timer 4
	TIM4->EGR |= TIM_EGR_UG; // Reinitialize the counter
	TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // PWM mode 1 (Upcounting)
	TIM4->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1FE; // Preload Enable, Fast Enable
	//TIM4->CCER |= TIM_CCER_CC1E; 	// Enable CH1 on output pin PA6
	//TIM4->CCER |= TIM_CCER_CC2E;	// Enable CH2 on output pin PA7
	TIM4->PSC = 0x095F; // Divide 24 MHz by 2400 (PSC+1), PSC_CLK= 10000 Hz, 1 count = 0.1 ms
	TIM4->ARR = 200; // 200 counts = 20 ms or 50 Hz
	TIM4->CCR1 = 5;	// 5 counts = 0.5 ms = 2.5% duty cycle = 0 degree axis
	TIM4->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; // Enable Timer 4
	*/
}

// Initialize SYSTICK (and setup interrupts??)
void enableTimer3IRQ(void) {
	
	TIM3->DIER |= TIM_DIER_CC1IE;
	NVIC_EnableIRQ(TIM3_IRQn);	// Enable inturrupt

}

void TIM3_IRQHandler(void) {

	// Update duty cycle for S1 (forward/backward control) on PA6 (header D12, TIM3 channel 1) based on input from PC1 (header A4, ADC channel 11)
	TIM3->CCR1 = (20 - (uint16_t)(adcRead(11) * 11 / 4095));
	
	// Update duty cycle for S2 (right/left control) on PA7 (header D11, TIM3 channel 2) based on input from PC0 (header A5, ADC channel 10)
	TIM3->CCR2 = (10 + (uint16_t)(adcRead(10) * 11 / 4095));

	TIM3->SR &= ~TIM_SR_CC1IF;	// Clear timer
}

