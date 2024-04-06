#include "../../project_types.h"
#include "../include/tasks_ese.h"

#include "../include/adc_ese.h"
#include "../include/motor_ese.h"

_Noreturn void motor_control_task(void* param){
    uint32_t joystick_y;
    uint32_t joystick_x;
    uint32_t chair_events;
    uint32_t switch_position;
    uint16_t s1_pulse = 15;
    uint16_t s2_pulse = 15;
    uint8_t stop_count = 0;
    uint8_t slowed = 0;
    
    while(1){
        chair_events = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        switch_position = GPIOC->IDR & GPIO_IDR_IDR7;
        
        if(chair_events & STOP_FROM_ULTRA) stop_count++;
        if(chair_events & STOP_FROM_USART) stop_count++;
        if(chair_events & CLEAR_STOP_FROM_ULTRA) stop_count--;
        if(chair_events & CLEAR_STOP_FROM_USART) stop_count--;
        if(chair_events & SLOW_CHAIR_NOTIFY) slowed = 1;
        if(chair_events & RESUME_SPEED_NOTIFY) slowed = 0;
        
        if(slowed == 1){
            joystick_y = read_joystick_y();
            joystick_x = read_joystick_x();
            if(joystick_y < 0x800) joystick_x = (~joystick_x) & 0xFFF;
            s1_pulse = (uint16_t)(12 + (joystick_y * 6 / 4095));
            s2_pulse = (uint16_t)(12 + (joystick_x * 6 / 4095));
        }
        
        if(stop_count != 0){
            s1_pulse = 15;
            s2_pulse = 15;
        }
        
        if((stop_count == 0 && slowed == 0) || (switch_position == 0)){
            joystick_y = read_joystick_y();
            joystick_x = read_joystick_x();
            if(joystick_y < 0x800) joystick_x = (~joystick_x) & 0xFFF;
            s1_pulse = (uint16_t)(10 + (joystick_y * 10 / 4095));
            s2_pulse = (uint16_t)(10 + (joystick_x * 10 / 4095));
        }
        
        /* Values preloaded, they are applied on the next PWM period */
        TIM3->CCR1 = s2_pulse;
        TIM3->CCR2 = s1_pulse;
        
        (void)param;
    }
}
