#include "../../project_types.h"
#include "../include/tasks_ese.h"

#include "../include/adc_ese.h"
#include "../include/motor_ese.h"

_Noreturn void motor_control_task(void* param){
    uint32_t joystick_y;
    uint32_t joystick_x;
    uint32_t notify_value;
    uint32_t switch_position;
    uint16_t s1_pulse = 15;
    uint16_t s2_pulse = 15;
    uint8_t stopped = 0;
    uint8_t slowed = 0;
    
    while(1){
        notify_value = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        switch_position = GPIOC->IDR & GPIO_IDR_IDR7;
        
        if(notify_value == USART_STOP_CHAIR){
            stop_joystick_read();
            s1_pulse = 15;
            s2_pulse = 15;
            stopped = 1;
        }
        
        else if(notify_value == USART_CLEAR_ERROR){
            start_joystick_read();
            stopped = 0;
        }
        
        else if(notify_value == RESUME_SPEED_NOTIFY){
            slowed = 0;
        }
        
        //play with values, see what works
        else if(notify_value == SLOW_SPEED_NOTIFY || slowed == 1){
            joystick_y = read_joystick_y();
            joystick_x = read_joystick_x();
            if(joystick_y < 0x800) joystick_x = (~joystick_x) & 0xFFF;
            s1_pulse = (uint16_t)(12 + (joystick_y * 6 / 4095));
            s2_pulse = (uint16_t)(12 + (joystick_x * 6 / 4095));
            slowed = 1;
        }
        
        if((stopped != 1 && slowed != 1) || (switch_position == 0)){
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
