#ifndef __GPIO_CLASS_H__
#define __GPIO_CLASS_H__

#include "user_main.h"

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
		GPIO_TypeDef* get_port(void) { return port; }
		uint16_t get_pin(void) { return pin; }

	MCU_GPIO(GPIO_TypeDef* port, uint16_t pin);
	~MCU_GPIO();

};

#endif