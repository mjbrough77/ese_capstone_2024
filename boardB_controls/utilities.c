/**************************************** 
Capstone Utilities
ENEL 417

Name: Emily Schwab
SID: 200426400
Date Created: 2024-01-19
****************************************/

#include "utilities.h"
#include "stm32f10x.h"

// Initialize all clocks, system clock with speed indicated by provided parameter (in MHz)
void clkInit(int speed)		
{
	// ***** Code sequence taken from Dave's code *****
	
	// enable HSI and wait for it to be ready
	RCC->CR |= RCC_CR_HSION;
	while (((RCC->CR) & (RCC_CR_HSION | RCC_CR_HSIRDY)) == 0);
	
	// enable HSE with bypass and wait for it to be ready
	RCC->CR |= RCC_CR_HSEON | RCC_CR_HSEBYP;
	while (((RCC->CR) & (RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_HSERDY)) == 0);
	
	// set HSE as SYSCLK and wait for it to be recognized
	RCC->CFGR = RCC_CFGR_SW_HSE;
  while (((RCC->CFGR) & (RCC_CFGR_SW_HSE | RCC_CFGR_SWS_HSE)) == 0);
	
	// Disable PLL. 	
	RCC->CR &= ~RCC_CR_PLLON;
	
	// set SYSCLK value based on speed
	switch(speed)
	{
		case 16:
			RCC->CFGR = 0x00010000;// 16 MHz
			break;
		case 32:
			RCC->CFGR = 0x00090000;// 32 MHz
			break;
		case 40:
			RCC->CFGR = 0x000d0000;// 40 MHz
			break;
		case 48:
			RCC->CFGR = 0x00110000;// 48 MHz
			break;
		case 56:
			RCC->CFGR = 0x00150000;//	56 MHz
			break;
		case 64:
			RCC->CFGR = 0x00190000;// 64 MHz
			break;
		case 72:
			RCC->CFGR = 0x001d0000;// 72 MHz	
			break;
		case 24:
		default:
			RCC->CFGR = 0x00050000;		// 24MHz
			break;
	}
	
	// enable PLL and wait for it to be ready
	RCC->CR |= RCC_CR_PLLON;
  while (((RCC->CR) & (RCC_CR_PLLON | RCC_CR_PLLRDY)) == 0);
	
	// set PLL and SYSCLK and wait for it to be ready
	RCC->CFGR |= RCC_CFGR_SW_PLL;// 0x00000002;
  while (((RCC->CFGR) & (RCC_CFGR_SW_PLL | RCC_CFGR_SWS_PLL)) == 0);
	
	// ***** Enable clocks *****
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;		// Enable AFIO clock
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;		// Enable Port A clock
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;		// Enable Port B clock
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;		// Enable Port C clock
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		// Enable ADC1 clock
	//RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;		// Enable TIM1 clock
	//RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;		// Enable TIM2 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;		// Enable TIM3 clock
	//RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;		// Enable TIM4 clock
	//RCC->APB2ENR |= RCC_APB2ENR_USART1EN;	// Enable USART1 clock
	//RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;		// Enable SPI2 clock
	
}

// Initialize Required I/O Pins/functions
void ioInit(void)
{
	/******************** PORT A ********************/
	
	// PA0 - ADC1 Input Pin on A0, Configure as Analog Input
	//		MODE = 00 (Input Mode)
	//		CNF = 00 (Analog Input)
	//GPIOA->CRL &= ~GPIO_CRL_MODE0 & ~GPIO_CRL_CNF0;		// clear MODE and CNF bits
	
	// PA1 - ADC1 Input Pin on A1, Configure as Analog Input
	//		MODE = 00 (Input Mode)
	//		CNF = 00 (Analog Input)
	//GPIOA->CRL &= ~GPIO_CRL_MODE1 & ~GPIO_CRL_CNF1;		// clear MODE and CNF bits
	
	// PA1 - TIM2 CH2 Output Pin on A1, Configure as Alternate Function Output, Push/Pull
	// 		MODE = 11 (Output Mode, Maximum 50MHz)
	// 		CNF = 10 (Alternate Function Output Push-Pull)
	GPIOA->CRL |= GPIO_CRL_MODE1 | GPIO_CRL_CNF1_1;			// PA1, (TIM2 CH2) set MODE bits and CNF bit 0
  GPIOA->CRL &= ~GPIO_CRL_CNF1_0;											// PA1, (TIM2 CH2) clear CNF bit 1
	
	// PA4 - ADC1 Input Pin on A2, Configure as Analog Input
	//		MODE = 00 (Input Mode)
	//		CNF = 00 (Analog Input)
	//GPIOA->CRL &= ~GPIO_CRL_MODE4 & ~GPIO_CRL_CNF4;		// clear MODE and CNF bits
	
	// PA4 - GPIO Output on A2, Configure as General Output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	GPIOA->CRL |= GPIO_CRL_MODE4;		// set MODE bits
	GPIOA->CRL &= ~GPIO_CRL_CNF4;		// clear CNF bits
	
	// PA5 - On-board LED Output (and D13), Configure as General Output, Push/Pull
	//		MODE = 11 (Output Mode, Maximum 50MHz)
	//		CNF = 00 (General Purpose Output Push-Pull)
	GPIOA->CRL |= GPIO_CRL_MODE5;		// set MODE bits
  GPIOA->CRL &= ~GPIO_CRL_CNF5;		// clear CNF bits
	
	// PA6 - GPIO Input on Pin D12, Configure as Floating Input
	//		MODE = 00 (Input Mode)
	//		CNF = 01 (Floating Mode)
	//GPIOA->CRL |= GPIO_CRL_CNF6_0;	// set CNF bit 0
	//GPIOA->CRL &= ~GPIO_CRL_CNF6_1 & ~GPIO_CRL_MODE6;		// clear CNF bit 1 and MODE bits
	
	// PA6 - GPIO Output on D12, Configure as General Output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	//GPIOA->CRL |= GPIO_CRL_MODE6;		// set MODE bits
	//GPIOA->CRL &= ~GPIO_CRL_CNF6;		// clear CNF bits
	
	// PA6 - TIM3 CH1 Output Pin on D12, Configure as Alternate Function Output, Push/Pull
	// 		MODE = 11 (Output Mode, Maximum 50MHz)
	// 		CNF = 10 (Alternate Function Output Push-Pull)
	GPIOA->CRL |= GPIO_CRL_MODE6 | GPIO_CRL_CNF6_1;			// PA6, (TIM3 CH1) set MODE bits and CNF bit 0
  GPIOA->CRL &= ~GPIO_CRL_CNF6_0;											// PA6, (TIM3 CH1) clear CNF bit 1
		
	// PA7 - GPIO Output on D11, Configure as General Output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	//GPIOA->CRL |= GPIO_CRL_MODE7;		// set MODE bits
	//GPIOA->CRL &= ~GPIO_CRL_CNF7;		// clear CNF bits
	
	// PA7 - TIM3 CH2 Output Pin on D11, Configure as Alternate Function Output, Push/Pull
	// 		MODE = 11 (Output Mode, Maximum 50MHz)
	// 		CNF = 10 (Alternate Function Output Push-Pull)
	GPIOA->CRL |= GPIO_CRL_MODE7 | GPIO_CRL_CNF7_1;			// PA7, (TIM3 CH2) set MODE bits and CNF bit 0
  GPIOA->CRL &= ~GPIO_CRL_CNF7_0;											// PA7, (TIM3 CH2) clear CNF bit 1
	
	// PA8 - GPIO Output on D7, Configure as General output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	//GPIOA->CRH |= GPIO_CRH_MODE8;		// set MODE bits
	//GPIOA->CRH &= ~GPIO_CRH_CNF8;		// clear CNF bits
	
	// PA9 - GPIO Output on D8, Configure as General output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	GPIOA->CRH |= GPIO_CRH_MODE9;		// set MODE bits
	GPIOA->CRH &= ~GPIO_CRH_CNF9;		// clear CNF bits
	
	// PA9 - USART1 Transmit Pin (TXD) on D8, Configure as AFIO Output, Push/Pull
	//		MODE = 11 (Output Mode, Maximum 50MHz) 
	//		CNF = 10 (Alternate Funciton Output Push-Pull)
	//GPIOA->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_CNF9_1;		// set MODE bits and CNF bit 1
	//GPIOA->CRH &= ~GPIO_CRH_CNF9_0;		// clear CNF bit 0

	// PA10 - GPIO Output on D2, Configure as General output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	GPIOA->CRH |= GPIO_CRH_MODE10;	// set MODE bits
	GPIOA->CRH &= ~GPIO_CRH_CNF10;		// clear CNF bits
	
	// PA10 - TIM1 CH3 Output Pin on D2, Configure as Alternate Function Output, Push/Pull
	// 		MODE = 11 (Output Mode, Maximum 50MHz)
	// 		CNF = 10 (Alternate Function Output Push-Pull)
	//GPIOA->CRH |= GPIO_CRH_MODE10 | GPIO_CRH_CNF10_1;			// PA10, (TIM1 CH3) set MODE bits and CNF bit 0
  //GPIOA->CRH &= ~GPIO_CRH_CNF10_0;											// PA10, (TIM1 CH3) clear CNF bit 1
	
	// PA10 - USART1 Receive Pin (RXD) or GPIO Input on D2, Configure as Floating Input
	// 		MODE = 00 (Input Mode)
	//		CNF = 01 (Floating Input)
	//GPIOA->CRH |= GPIO_CRH_CNF10_0;		// set CNF bit 0
	//GPIOA->CRH &= ~GPIO_CRH_MODE10 | ~GPIO_CRH_CNF10_1;		// clear MODE bits and CNF bit 1
	
	/******************** PORT B ********************/
	
	// PB0 - ADC1 Input Pin on A3, Configure as Analog Input
	//		MODE = 00 (Input Mode)
	//		CNF = 00 (Analog Input)
	GPIOB->CRL &= ~GPIO_CRL_MODE0 & ~GPIO_CRL_CNF0;		// clear MODE and CNF bits

	// PB0 - GPIO Input on Pin A3, Configure as Floating Input
	// 		MODE = 00 (Input Mode)
	//		CNF = 01 (Floating Input)
	//GPIOB->CRL |= GPIO_CRL_CNF0_0;										// set CNF bit 0
	//GPIOB->CRL &= ~GPIO_CRL_CNF0_1 | ~GPIO_CRL_MODE4;		// clear MODE bits and CNF bit 1

	// PB4 - GPIO Input on Pin D5, Configure as Floating Input
	// 		MODE = 00 (Input Mode)
	//		CNF = 01 (Floating Input)
	//GPIOB->CRL |= GPIO_CRL_CNF4_0;										// set CNF bit 0
	//GPIOB->CRL &= ~GPIO_CRL_CNF4_1 | ~GPIO_CRL_MODE4;		// clear MODE bits and CNF bit 1
	
	// PB4 - GPIO Output on Pin D5, Configure as General Output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;	// Remap
	GPIOB->CRL |= GPIO_CRL_MODE4;		// set MODE bits ???
	GPIOB->CRL &= ~GPIO_CRL_CNF4;		// clear CNF bits ???
	
	// PB5 - GPIO Input on Pin D4, Configure as Floating Input
	//	MODE = 00 (Input Mode)
	//	CNF - 01 (Floating Mode)
	//GPIOB->CRL |= GPIO_CRL_CNF5_0;										// set CNF bit 0
	//GPIOB->CRL &= ~GPIO_CRL_CNF5_1 | ~GPIO_CRL_MODE5;		// clear MODE bits and CNF bit 1
	
	// PB5 - GPIO Output on Pin D4, Configure as General Output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	GPIOB->CRL |= GPIO_CRL_MODE5;		// set MODE bits
	GPIOB->CRL &= ~GPIO_CRL_CNF5;		// clear CNF bits
	
	// PB6 - GPIO Output on Pin D10, Configure as General Output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	//GPIOB->CRL |= GPIO_CRL_MODE6;		// set MODE bits
	//GPIOB->CRL &= ~GPIO_CRL_CNF6;		// clear CNF bits
	
	// PB8 - GPIO Input on Pin D15, Configure as Floating Input
	//	MODE = 00 (Input Mode)
	//	CNF - 01 (Floating Mode)
	//GPIOB->CRH |= GPIO_CRH_CNF8_0;										// set CNF bit 0
	//GPIOB->CRH &= ~GPIO_CRH_CNF8_1 | ~GPIO_CRH_MODE8;		// clear MODE bits and CNF bit 1
	
	// PB8 - GPIO Output on Pin D15, Configure as General Output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	GPIOB->CRH |= GPIO_CRH_MODE8;		// set MODE bits
	GPIOB->CRH &= ~GPIO_CRH_CNF8;		// clear CNF bits
	
	// PB9 - GPIO Input on Pin D14, Configure as Floating Input
	//	MODE = 00 (Input Mode)
	//	CNF - 01 (Floating Mode)
	//GPIOB->CRH |= GPIO_CRH_CNF9_0;										// set CNF bit 0
	//GPIOB->CRH &= ~GPIO_CRH_CNF9_1 | ~GPIO_CRH_MODE9;		// clear MODE bits and CNF bit 1
	
	// PB9 - GPIO Output on Pin D14, Configure as General Output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	GPIOB->CRH |= GPIO_CRH_MODE9;		// set MODE bits
	GPIOB->CRH &= ~GPIO_CRH_CNF9;		// clear CNF bits
	
	// PB10 - GPIO Output on Pin D6, Configure as General Output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	GPIOB->CRH |= GPIO_CRH_MODE10;		// set MODE bits
	GPIOB->CRH &= ~GPIO_CRH_CNF10;		// clear CNF bits
	
	// PB12 - SPI2 Negative Slave Select (NSS) on ??, Configure as GPIO Output, Push/Pull
	//	MODE = 11 (Output Mode, Maximum 50MHz)
	//	CNF = 00 (General Purpose Output Push-Pull)
	//GPIOB->CRH |= GPIO_CRH_MODE12;	// set MODE bits
	//GPIOB->CRH &= ~GPIO_CRH_CNF12;		// clear CNF bits
	
	// PB13 - SPI2 System Clock (SCK) on ??, Configure as AFIO Output, Push/Pull
	//		MODE = 11 (Output Mode, Maximum 50MHz) 
	//		CNF = 10 (Alternate Funciton Output Push-Pull)
	//GPIOB->CRH |= GPIO_CRH_MODE13 | GPIO_CRH_CNF13_1;		// set MODE bits and CNF bit 1
	//GPIOB->CRH &= ~GPIO_CRH_CNF13_0;		// clear CNF bit 0
	
	// PB14 - SPI2 Master out Slave in (MOSI) on ??, Configure as GPIO Input, (Floating?)
	// 		MODE = 00 (Input Mode)
	//		CNF = 01 (Floating Input)
	//GPIOB->CRH |= GPIO_CRH_CNF14_0;		// set CNF bit 0
	//GPIOB->CRH &= ~GPIO_CRH_MODE14 | ~GPIO_CRH_CNF14_1;		// clear MODE bits and CNF bit 1
	
	// PB15 - SPI2 Master in Slave out (MISO) on ??, Configure as AFIO Output, Push/Pull
	//		MODE = 11 (Output Mode, Maximum 50MHz) 
	//		CNF = 10 (Alternate Funciton Output Push-Pull)
	//GPIOB->CRH |= GPIO_CRH_MODE15 | GPIO_CRH_CNF15_1;		// set MODE bits and CNF bit 1
	//GPIOB->CRH &= ~GPIO_CRH_CNF15_0;		// clear CNF bit 0
	
	/******************** PORT C ********************/
	
	// PC0 - ADC1 Input Pin on A5, Configure as Analog Input
	//		MODE = 00 (Input Mode)
	//		CNF = 00 (Analog Input)
	GPIOC->CRL &= ~GPIO_CRL_MODE0 & ~GPIO_CRL_CNF0;		// clear MODE and CNF bits
	
	// PC1 - ADC1 Input Pin on A4, Configure as Analog Input
	//		MODE = 00 (Input Mode)
	//		CNF = 00 (Analog Input)
	GPIOC->CRL &= ~GPIO_CRL_MODE1 & ~GPIO_CRL_CNF1;		// clear MODE and CNF bits
	
	// PC7 - GPIO Input on Pin D9, Configure as Floating Input
	// 		MODE = 00 (Input Mode)
	//		CNF = 01 (Floating Input)
	GPIOC->CRL |= GPIO_CRL_CNF7_0;										// set CNF bit 0
	GPIOC->CRL &= ~GPIO_CRL_CNF7_1 | ~GPIO_CRL_MODE7;		// clear MODE bits and CNF bit 1

	// PC13 - On-board USER Button Input, Configure as Floating Input
	//	MODE = 00 (Input Mode)
	//	CNF - 01 (Floating Mode)
	//GPIOC->CRH |= GPIO_CRH_CNF13_0;	// set CNF bit 0
	//GPIOC->CRH &= ~GPIO_CRH_CNF13_1 | ~GPIO_CRH_MODE13;		// clear MODE bits and CNF bit 1

}

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

