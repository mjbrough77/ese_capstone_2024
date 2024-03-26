#include "../../project_types.h"
#include "../include/tasks_ese.h"

#include "../include/adc_ese.h"
#include "../include/motor_ese.h"

_Noreturn void motor_control_task(void* param){
    uint32_t joystick_y;
    uint32_t joystick_x;
    uint32_t notify_value;
    uint16_t s1_pulse = 15;
    uint16_t s2_pulse = 15;
    uint8_t stopped = 0;
//    uint32_t movement_upper_limit = 20;
//    uint32_t movement_lower_limit = 10;
    
    while(1){
        notify_value = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
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
        
        else if(stopped != 1){
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
