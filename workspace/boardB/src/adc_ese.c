#include "../../project_types.h"
#include "../include/tasks_ese.h"

#include "../include/adc_ese.h"

void configure_adc1(void){  
	ADC1->CR2 |= ADC_CR2_ADON;      /* Enable ADC1 */
    ADC1->CR2 |= ADC_CR2_CONT;      /* Perform continuous channel scanning */
    
	ADC1->CR2 |= ADC_CR2_CAL;       /* Calibrate ADC1 */
    while(ADC1->CR2 & ADC_CR2_CAL);
    ADC1->CR2 |= ADC_CR2_ADON;      /* Start ADC conversions */
}

void get_weight_task(void* param){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    Weight_t user_weight;
    uint32_t adc_reading;
    float voltage_reading;
    float voltage_error = 0.0f;
    uint32_t error_count = 300;
    
    while(1){
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS( WEIGHT_SAMPLE_MS ));
        adc_reading = ADC1->DR;
        voltage_reading = (float)adc_reading * ADC_RESOLUTION;
        
        voltage_error += voltage_reading;
        error_count--;
        
        if(error_count == 0){
            voltage_error /= 300.0f;
            while(1);
        }
        
        // Do testing to figure out calculations
        
        (void)param;
    }
}
