#ifndef __USER_MAIN_H__
#define __USER_MAIN_H__

#include "main.h"
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




#endif /* USER CODE END Header */