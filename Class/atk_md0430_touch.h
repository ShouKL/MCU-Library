/**
 ****************************************************************************************************
 * @file        atk_md0430_touch.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MD0430ģ�鴥����������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� M144Z-M4��Сϵͳ��STM32F407��
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __ATK_MD0430_TOUCH_H
#define __ATK_MD0430_TOUCH_H

#include "./BSP/ATK_MD0430/atk_md0430.h"

/* ���Ŷ��� */
#define ATK_MD0430_TOUCH_PEN_GPIO_PORT          GPIOB
#define ATK_MD0430_TOUCH_PEN_GPIO_PIN           GPIO_PIN_1
#define ATK_MD0430_TOUCH_PEN_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)
#define ATK_MD0430_TOUCH_TCS_GPIO_PORT          GPIOC
#define ATK_MD0430_TOUCH_TCS_GPIO_PIN           GPIO_PIN_13
#define ATK_MD0430_TOUCH_TCS_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)

/* IO���� */
#define ATK_MD0430_TOUCH_READ_PEN()             HAL_GPIO_ReadPin(ATK_MD0430_TOUCH_PEN_GPIO_PORT, ATK_MD0430_TOUCH_PEN_GPIO_PIN)
#define ATK_MD0430_TOUCH_PEN(x)                 do{ x ?                                                                                                 \
                                                    HAL_GPIO_WritePin(ATK_MD0430_TOUCH_PEN_GPIO_PORT, ATK_MD0430_TOUCH_PEN_GPIO_PIN, GPIO_PIN_SET) :    \
                                                    HAL_GPIO_WritePin(ATK_MD0430_TOUCH_PEN_GPIO_PORT, ATK_MD0430_TOUCH_PEN_GPIO_PIN, GPIO_PIN_RESET);   \
                                                }while(0)
#define ATK_MD0430_TOUCH_TCS(x)                 do{ x ?                                                                                                 \
                                                    HAL_GPIO_WritePin(ATK_MD0430_TOUCH_TCS_GPIO_PORT, ATK_MD0430_TOUCH_TCS_GPIO_PIN, GPIO_PIN_SET) :    \
                                                    HAL_GPIO_WritePin(ATK_MD0430_TOUCH_TCS_GPIO_PORT, ATK_MD0430_TOUCH_TCS_GPIO_PIN, GPIO_PIN_RESET);   \
                                                }while(0)

/* ATK-MD0430����IICͨѶ��ַö�� */
typedef enum
{
    ATK_MD0430_TOUCH_IIC_ADDR_14 = 0x14,    /* 0x14 */
    ATK_MD0430_TOUCH_IIC_ADDR_5D = 0x5D,    /* 0x5D */
} atk_md0430_touch_iic_addr_t;

/* �������������ݽṹ */
typedef struct
{
    uint16_t x;     /* ������X���� */
    uint16_t y;     /* ������Y���� */
    uint16_t size;  /* �������С */
} atk_md0430_touch_point_t;

/* ������� */
#define ATK_MD0430_TOUCH_EOK                    0   /* û�д��� */
#define ATK_MD0430_TOUCH_ERROR                  1   /* ���� */

/* �������� */
uint8_t atk_md0430_touch_init(void);                                            /* ATK-MD0430ģ�鴥����ʼ�� */
uint8_t atk_md0430_touch_scan(atk_md0430_touch_point_t *point, uint8_t cnt);    /* ATK-MD0430ģ�鴥��ɨ�� */

#endif

