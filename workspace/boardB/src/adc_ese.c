#include "../../project_types.h"
#include "../include/tasks_ese.h"

#include "../include/adc_ese.h"

void configure_adc1(void){  
    ADC1->CR2 |= ADC_CR2_ADON;      /* Enable ADC1 */
    ADC1->CR2 |= ADC_CR2_CONT;      /* Continuous conversion */
    ADC1->SMPR1 |= ADC_SMPR1_SMP10; /* Sample time = 12.6us @ 20MHz */
    ADC1->SQR3 = 0xA;               /* Convert IN10 (PC0) */
    
	ADC1->CR2 |= ADC_CR2_CAL;       /* Calibrate ADC1 */
    while(ADC1->CR2 & ADC_CR2_CAL);
    
    ADC1->CR2 |= ADC_CR2_ADON;      /* Start ADC conversions */
}

void get_weight_task(void* param){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    float user_weight;
    uint32_t adc_reading;
    float voltage_reading;
    
    while(1){
    #ifdef WEIGHT_TASK_SUSPEND
        vTaskSuspend(NULL);
    #endif
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS( WEIGHT_SAMPLE_MS ));
        
        adc_reading = ADC1->DR;
        voltage_reading = (float)adc_reading * ADC_RESOLUTION - WEIGHT_TARE;
        user_weight = voltage_reading / WEIGHT_RESOLUTION;
        
        (void)param;
    }
}
