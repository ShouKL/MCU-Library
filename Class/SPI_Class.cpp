#include "SPI_Class.h"

bool MCU_SPI::transmit(uint8_t *data, uint16_t size, uint32_t timeout)
{
    if(HAL_SPI_Transmit(hspi, data, size, timeout) != HAL_OK)
    {
      Error_Handler();
      return false;
    }
    return true;
}

bool MCU_SPI::receive(uint8_t *data, uint16_t size, uint32_t timeout)
{
    if(HAL_SPI_Receive(hspi, data, size, timeout) != HAL_OK)
    {
      Error_Handler();
      return false;
    }
    return true;
}

bool MCU_SPI::transmitByte(uint8_t *data, uint32_t timeout)
{
    return transmit(data, 1, timeout);
}

bool MCU_SPI::receiveByte(uint8_t *data, uint32_t timeout)
{
    return receive(data, 1, timeout);
}

MCU_SPI::MCU_SPI(SPI_HandleTypeDef *hspi, MCU_GPIO cs) : hspi(hspi), CS(cs)
{
}

MCU_SPI::~MCU_SPI()
{
}
