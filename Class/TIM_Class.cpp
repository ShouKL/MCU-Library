#include "TIM_Class.h"

void MCU_TIM::start_interrupt(void)
{
    HAL_TIM_Base_Start_IT(htim);
}

void MCU_TIM::stop_interrupt(void)
{
    HAL_TIM_Base_Stop_IT(htim);
}

void MCU_TIM::start_pwm(void)
{
    HAL_TIM_PWM_Start(htim, channel);
}

void MCU_TIM::stop_pwm(void)
{
    HAL_TIM_PWM_Stop(htim, channel);
}

void MCU_TIM::set_compare(uint16_t compare)
{
    m_compare = compare;
    __HAL_TIM_SET_COMPARE(htim, channel, m_compare);
}

MCU_TIM::MCU_TIM(TIM_HandleTypeDef *htim, uint32_t channel) : htim(htim), channel(channel)
{
}

MCU_TIM::~MCU_TIM()
{
}
