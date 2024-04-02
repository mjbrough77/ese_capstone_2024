#include "../include/adc_ese.h"

void configure_adc1(void){  
	ADC1->CR2 |= ADC_CR2_ADON;      /* Enable ADC1 */
    ADC1->CR1 |= ADC_CR1_SCAN;      /* Convert multiple analog channels */
    ADC1->CR2 |= ADC_CR2_CONT;      /* Perform continuous channel scanning */
    ADC1->CR1 |= ADC_CR1_JAUTO;     /* Automatically convert inj channels */
    ADC1->JSQR = 0x15A800;          /* Conversion order: PC0->PC1 */
    
	ADC1->CR2 |= ADC_CR2_CAL;       /* Calibrate ADC1 */
    while(ADC1->CR2 & ADC_CR2_CAL);
}

void start_joystick_read(void){
    ADC1->CR2 |= ADC_CR2_ADON;      /* Start ADC conversions */
    NVIC_EnableIRQ(TIM3_IRQn);      /* Start data conversions */
    ADC1->CR2 |= ADC_CR2_ADON;
}

void stop_joystick_read(void){
    ADC1->CR2 &= ~ADC_CR2_ADON;     /* Stop ADC conversions */
    NVIC_DisableIRQ(TIM3_IRQn);     /* Stop data conversions */              
}

uint32_t read_joystick_x(void){
    return ADC1->JDR1;              /* Word access is atomic */
}

uint32_t read_joystick_y(void){
    return ADC1->JDR2;              /* Word access is atomic */
}
