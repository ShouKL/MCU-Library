#include "LED_Class.h"

void LED::breath(uint32_t target_arr, uint32_t steps, uint32_t duration_ms)
{
    if (steps == 0) return;

    uint32_t interval = duration_ms / (steps * 2);
    if (interval == 0) interval = 1; 

    for (uint32_t i = 0; i <= steps; i++)
    {
        uint32_t compare_val = (uint32_t)((uint64_t)i * target_arr / steps);
        MCU_TIM::set_compare(compare_val);
        Delay(interval);
    }

    for (uint32_t i = steps; i > 0; i--)
    {
        uint32_t compare_val = (uint32_t)((uint64_t)i * target_arr / steps);
        MCU_TIM::set_compare(compare_val);
        Delay(interval);
    }
    
    MCU_TIM::set_compare(0);
}

void LED::breath(LED leds[], uint32_t size, uint32_t target_arr, uint32_t steps, uint32_t duration_ms)
{
    if (steps == 0) return;
    uint32_t interval = duration_ms / (steps * 2);
    if (interval == 0) interval = 1; 

    for (uint32_t i = 0; i <= steps; i++)
    {
        uint32_t compare_val = (uint32_t)(i * target_arr / steps);

        for (uint32_t j = 0; j < size; j++) {
            leds[j].set_compare(compare_val);
        }
        Delay(interval);
    }

    for (int32_t i = steps; i >= 0; i--)
    {
        uint32_t compare_val = (uint32_t)(i * target_arr / steps);
        for (uint32_t j = 0; j < size; j++) {
            leds[j].set_compare(compare_val);
        }
        Delay(interval);
    }
}


LED::LED(GPIO_TypeDef* port, uint16_t pin) : MCU_GPIO(port, pin), MCU_TIM(nullptr, NULL)
{
}

LED::LED(TIM_HandleTypeDef *htim, uint32_t channel) : MCU_GPIO(nullptr, NULL), MCU_TIM(htim, channel)
{
}

LED::~LED()
{
}
