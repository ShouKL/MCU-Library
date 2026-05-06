#ifndef __LED_CLASS_H__
#define __LED_CLASS_H__

#include "user_main.h"
#include "GPIO_Class.h"
#include "TIM_Class.h"

class LED : public MCU_GPIO, public MCU_TIM
{
	private:
	
	protected:
		
	public:
		void breath(uint32_t target_arr, uint32_t steps, uint32_t duration_ms);
		static void breath(LED leds[], uint32_t size, uint32_t target_arr, uint32_t steps, uint32_t duration_ms);

	LED(GPIO_TypeDef* port, uint16_t pin);
	LED(TIM_HandleTypeDef* htim, uint32_t channel);
	~LED();

};

#endif