#include "GPIO_Class.h"

void MCU_GPIO::high(void)
{
    HAL_GPIO_WritePin((GPIO_TypeDef *)port, pin, GPIO_PIN_SET);
}

void MCU_GPIO::low(void)
{
    HAL_GPIO_WritePin((GPIO_TypeDef *)port, pin, GPIO_PIN_RESET);
}

uint8_t MCU_GPIO::read(void)
{
    return HAL_GPIO_ReadPin((GPIO_TypeDef *)port, pin);
}

MCU_GPIO::MCU_GPIO(GPIO_TypeDef* port, uint16_t pin) : level(0), port(port), pin(pin)
{
}

MCU_GPIO::~MCU_GPIO()
{
}
