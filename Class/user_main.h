#ifndef __USER_MAIN_H__
#define __USER_MAIN_H__
#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <math.h>
//#include "arm_math.h"
#include <stddef.h>
#include "cmsis_os.h"
#include "cmsis_gcc.h"

#ifdef FREERTOS
#include "cmsis_os.h"
static inline void Delay(uint32_t ms)
{
    osDelay(ms);
}
#else
static inline void Delay(uint32_t ms)
{
    HAL_Delay(ms);
}
#endif

static inline void Init_Delay(uint32_t ms)
{
    HAL_Delay(ms);
}

static inline void Delay_us(uint32_t us)
{
    us *= (SystemCoreClock / 1000000) / 3;
    while (us--) __asm("nop");
}

#endif
