#ifndef __USER_MAIN_H__
#define __USER_MAIN_H__
#include "main.h"
#include "stm32f4xx_hal.h"       // 核心 HAL
#include <stdint.h>


#include "cJSON.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <math.h>
#include "arm_math.h"
#include <stddef.h>
#include "cmsis_os.h"
#include <cmath>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ATK_MD0430_Class.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

#define HAL

#ifdef HAL
#include "stm32f4xx_hal.h"       // 核心 HAL
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_can.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_crc.h"
#include "stm32f4xx_hal_dac.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_eth.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_i2s.h"
#include "stm32f4xx_hal_pcd.h"
#include "stm32f4xx_hal_pwr.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_sai.h"
#include "stm32f4xx_hal_sd.h"
#include "stm32f4xx_hal_smartcard.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_usart.h"
#include "stm32f4xx_hal_rng.h"
#include "stm32f4xx_hal_rtc.h"
#include "stm32f4xx_hal_iwdg.h"
#include "stm32f4xx_hal_smbus.h"
#include "stm32f4xx_hal_irda.h"
#endif

/* USER CODE END Includes */
//#define FREERTOS

/* Provide a header-only delay wrapper with internal linkage.
   When building with FreeRTOS, define `USE_FREERTOS` (project option)
   to map Delay() to `osDelay()`; otherwise use `HAL_Delay()`.

   If your project already includes CMSIS-RTOS2 headers, you can
   also omit defining USE_FREERTOS and instead include `cmsis_os.h`
   before this header. */
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

#endif /* USER CODE END Header */