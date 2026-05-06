#include "ADC_Class.h"

MCU_ADC::MCU_ADC(ADC_HandleTypeDef* hadc) : hadc(hadc), last_value(0)
{
}

MCU_ADC::~MCU_ADC()
{
    Stop();
}

HAL_StatusTypeDef MCU_ADC::Start()
{
    return HAL_ADC_Start(hadc);
}

HAL_StatusTypeDef MCU_ADC::Stop()
{
    return HAL_ADC_Stop(hadc);
}

uint32_t MCU_ADC::GetValue()
{
    if (HAL_ADC_PollForConversion(hadc, 10) == HAL_OK) {
        last_value = HAL_ADC_GetValue(hadc);
    }
    return last_value;
}