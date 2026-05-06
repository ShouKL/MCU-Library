#include "I2C_Class.h"

//写入数据到指定地址
bool MCU_I2C::write(uint8_t *data, uint16_t size, uint16_t Memaddress, uint32_t timeout)
{
    if(HAL_I2C_Mem_Write(hi2c, SLAVE_ADDRESS, Memaddress, MemAddSize, data, size, timeout) != HAL_OK)
    {
      Error_Handler();
      return false;
    }
    return true;
}

//发送数据
bool MCU_I2C::transmit(uint8_t* data, uint16_t size, uint32_t timeout)
{
	
    if(HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDRESS, data, size, timeout) != HAL_OK)
    {
        Error_Handler();
        return false;
    }
    return true;
}

//从指定地址读取数据
bool MCU_I2C::read(uint8_t *data, uint16_t size, uint16_t Memaddress, uint32_t timeout)
{
    if(HAL_I2C_Mem_Read(hi2c, SLAVE_ADDRESS, Memaddress, MemAddSize, data, size, timeout) != HAL_OK)
    {
      Error_Handler();
      return false;
    }
    return true;
}

//接收数据
bool MCU_I2C::receive(uint8_t* data, uint16_t size, uint32_t timeout)
{
    if(HAL_I2C_Master_Receive(hi2c, SLAVE_ADDRESS, data, size, timeout) != HAL_OK)
    {
        Error_Handler();
        return false;
    }
    return true;
}

MCU_I2C::MCU_I2C(I2C_HandleTypeDef *hi2c, uint8_t address, uint16_t memAddSize) 
            : hi2c(hi2c), SLAVE_ADDRESS(address), MemAddSize(memAddSize)
{
}

MCU_I2C::~MCU_I2C()
{
}
