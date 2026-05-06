#ifndef __SPI_CLASS_H__
#define __SPI_CLASS_H__

#include "user_main.h"
#include "GPIO_Class.h"

class MCU_SPI
{
	private:


	protected:
    	SPI_HandleTypeDef* hspi;
        MCU_GPIO CS;

	public:
		bool transmit(uint8_t* data, uint16_t size, uint32_t timeout = HAL_MAX_DELAY);
		bool receive(uint8_t* data, uint16_t size, uint32_t timeout = HAL_MAX_DELAY);

		bool transmitByte(uint8_t* data, uint32_t timeout = HAL_MAX_DELAY);
		bool receiveByte(uint8_t* data, uint32_t timeout = HAL_MAX_DELAY);

	MCU_SPI(SPI_HandleTypeDef* hspi, MCU_GPIO cs);
	~MCU_SPI();

};

#endif
