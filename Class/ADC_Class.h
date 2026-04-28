#ifndef __ADC_CLASS_H__
#define __ADC_CLASS_H__

#include "user_main.h"
#include <stdint.h>

class MCU_ADC
{
    private:

    protected:
        ADC_HandleTypeDef* hadc;
        uint32_t last_value;

    public:
        MCU_ADC(ADC_HandleTypeDef* hadc);
        ~MCU_ADC();

        HAL_StatusTypeDef Start();
        HAL_StatusTypeDef Stop();
        uint32_t GetValue();
};

#endif /* __ADC_CLASS_H__ */