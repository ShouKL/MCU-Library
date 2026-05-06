#ifndef __TIM_CLASS_H__
#define __TIM_CLASS_H__

#include "user_main.h"

class MCU_TIM
{
	private:
		
	
	protected:
		TIM_HandleTypeDef* htim;
		uint32_t channel;

		uint32_t m_compare;

	public:
		
		
	public:
		void start_interrupt(void);
		void stop_interrupt(void);
		void start_pwm(void);
		void stop_pwm(void);
		void set_compare(uint16_t compare);

	MCU_TIM(TIM_HandleTypeDef* htim, uint32_t channel = NULL);
	~MCU_TIM();

};

#endif
