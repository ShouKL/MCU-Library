#ifndef __I2C_CLASS_H__
#define __I2C_CLASS_H__

#include "user_main.h"
#include "i2c.h"

#include <stdint.h>

class MCU_I2C
{
	private:


	protected:
    	I2C_HandleTypeDef* hi2c;
        uint8_t SLAVE_ADDRESS;
        uint16_t REGISTER_ADDRESS;
        uint16_t MemAddSize;
		
	public:
		bool write(uint8_t* data, uint16_t size, uint16_t Memaddress, uint32_t timeout = HAL_MAX_DELAY);
		bool transmit(uint8_t* data, uint16_t size, uint32_t timeout = HAL_MAX_DELAY);
		bool read(uint8_t* data, uint16_t size, uint16_t Memaddress, uint32_t timeout = HAL_MAX_DELAY);
		bool receive(uint8_t* data, uint16_t size, uint32_t timeout = HAL_MAX_DELAY);

		bool writeByte(uint8_t *data, uint16_t Memaddress, uint32_t timeout = HAL_MAX_DELAY){ return write(data, 1, Memaddress, timeout); }
		bool transmitByte(uint8_t *data, uint32_t timeout = HAL_MAX_DELAY){ return transmit(data, 1, timeout); }
		bool readByte(uint8_t *data, uint16_t Memaddress, uint32_t timeout = HAL_MAX_DELAY) { return read(data, 1, Memaddress, timeout); }
		bool receiveByte(uint8_t *data, uint32_t timeout = HAL_MAX_DELAY) { return receive(data, 1, timeout); }

	MCU_I2C(I2C_HandleTypeDef* hi2c, uint8_t SLAVE_ADDRESS, uint16_t MemAddSize);
	~MCU_I2C();

};

#endif
