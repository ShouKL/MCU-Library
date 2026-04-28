#ifndef __GPIO_CLASS_H__
#define __GPIO_CLASS_H__

#include "user_main.h"
#include "gpio.h"

#include <stdint.h>

class MCU_GPIO
{
	private:
		uint8_t level;
	
	protected:
		
	public:
		GPIO_TypeDef* port;
		uint16_t pin;
		
	public:
		void high(void);
		void low(void);
		uint8_t read(void);
	
	MCU_GPIO(GPIO_TypeDef* port, uint16_t pin);
	~MCU_GPIO();

};

#endif