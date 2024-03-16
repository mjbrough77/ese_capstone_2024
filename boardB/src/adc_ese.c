#include "../include/adc_ese.h"

void configure_adc1(void){  
	ADC1->CR2 |= ADC_CR2_ADON;      /* Enable ADC1 */
    ADC1->CR1 |= ADC_CR1_SCAN;      /* Convert multiple analog channels */
    ADC1->CR2 |= ADC_CR2_CONT;      /* Perform continuous channel scanning */
    ADC1->CR1 |= ADC_CR1_JAUTO;     /* Automatically convert inj channels */
    ADC1->JSQR = 0x15A800;          /* Conversion order: PC0->PC1 */
    
	ADC1->CR2 |= ADC_CR2_CAL;       /* Calibrate ADC1 */
    while(ADC1->CR2 & ADC_CR2_CAL);
    ADC1->CR2 |= ADC_CR2_ADON;      /* Start ADC conversions */
}

uint32_t read_joystick_x(void){
    return ADC1->JDR1;  /* Word access is atomic */
}

uint32_t read_joystick_y(void){
    return ADC1->JDR2;  /* Word access is atomic */
}
