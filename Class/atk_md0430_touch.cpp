#include "atk_md0430_touch.h"
#include "atk_md0430_touch_iic.h"
#include "delay.h"
#include <string.h>

/* ATK-MD0430??PID */
#define ATK_MD0430_TOUCH_PID1    "911"
#define ATK_MD0430_TOUCH_PID2    "1158"

/* ATK-MD0430? */
#define ATK_MD0430_TOUCH_TP_MAX 5

/* ATK-MD0430???? */
#define ATK_MD0430_TOUCH_REG_CTRL           0x8040  /* ?? */
#define ATK_MD0430_TOUCH_REG_PID            0x8140  /* PID? */
#define ATK_MD0430_TOUCH_REG_TPINFO         0x814E  /* ??? */
#define ATK_MD0430_TOUCH_REG_TP1            0x8150  /* 1?? */
#define ATK_MD0430_TOUCH_REG_TP2            0x8158  /* 2?? */
#define ATK_MD0430_TOUCH_REG_TP3            0x8160  /* 3?? */
#define ATK_MD0430_TOUCH_REG_TP4            0x8168  /* 4?? */
#define ATK_MD0430_TOUCH_REG_TP5            0x8170  /* 5?? */

/* ??? */
#define ATK_MD0430_TOUCH_TPINFO_MASK_STA    0x80
#define ATK_MD0430_TOUCH_TPINFO_MASK_CNT    0x0F

/* ATK-MD0430???? */
static const uint16_t g_atk_md0430_touch_tp_reg[ATK_MD0430_TOUCH_TP_MAX] = {
    ATK_MD0430_TOUCH_REG_TP1,
    ATK_MD0430_TOUCH_REG_TP2,
    ATK_MD0430_TOUCH_REG_TP3,
    ATK_MD0430_TOUCH_REG_TP4,
    ATK_MD0430_TOUCH_REG_TP5,
};

/**
 * @brief       ATK-MD0430????
 * @param       
 * @retval      
 */
static void atk_md0430_touch_hw_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* ?? */
    ATK_MD0430_TOUCH_PEN_GPIO_CLK_ENABLE();
    ATK_MD0430_TOUCH_TCS_GPIO_CLK_ENABLE();
    
    /* ?PEN */
    gpio_init_struct.Pin    = ATK_MD0430_TOUCH_PEN_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_INPUT;
    gpio_init_struct.Pull   = GPIO_NOPULL;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0430_TOUCH_PEN_GPIO_PORT, &gpio_init_struct);
    
    /* ?TCS */
    gpio_init_struct.Pin    = ATK_MD0430_TOUCH_TCS_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull   = GPIO_PULLUP;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0430_TOUCH_TCS_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       ATK-MD0430????
 * @param       addr: ??õIIC???
 * @retval      
 */
static void atk_md0430_touch_hw_reset(atk_md0430_touch_iic_addr_t addr)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* PEN? */
    gpio_init_struct.Pin    = ATK_MD0430_TOUCH_PEN_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull   = GPIO_PULLUP;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0430_TOUCH_PEN_GPIO_PORT, &gpio_init_struct);
    
    switch (addr)
    {
        case ATK_MD0430_TOUCH_IIC_ADDR_14:
        {
            ATK_MD0430_TOUCH_TCS(0);
            ATK_MD0430_TOUCH_PEN(0);
            ATK_MD0430_TOUCH_PEN(1);
            delay_ms(1);
            ATK_MD0430_TOUCH_TCS(1);
            delay_ms(10);
            break;
        }
        case ATK_MD0430_TOUCH_IIC_ADDR_5D:
        {
            ATK_MD0430_TOUCH_TCS(0);
            ATK_MD0430_TOUCH_PEN(0);
            delay_ms(1);
            ATK_MD0430_TOUCH_TCS(1);
            delay_ms(10);
            break;
        }
        default:
        {
            break;
        }
    }
    
    /* PEN? */
    gpio_init_struct.Pin    = ATK_MD0430_TOUCH_PEN_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_INPUT;
    gpio_init_struct.Pull   = GPIO_NOPULL;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0430_TOUCH_PEN_GPIO_PORT, &gpio_init_struct);
}

/**
 * @brief       ATK-MD0430???
 * @param       
 * @retval      
 */
static void atk_md0430_touch_sw_reset(void)
{
    uint8_t dat;
    
    dat = 0x02;
    atk_md0430_touch_iic_write_reg(ATK_MD0430_TOUCH_REG_CTRL, &dat, 1);
    delay_ms(10);
    
    dat = 0x00;
    atk_md0430_touch_iic_write_reg(ATK_MD0430_TOUCH_REG_CTRL, &dat, 1);
}

/**
 * @brief       ?ATK-MD0430??PID
 * @param       pid: ?PIDASCII
 * @retval      
 */
static void atk_md0430_touch_get_pid(char *pid)
{
    atk_md0430_touch_iic_read_reg(ATK_MD0430_TOUCH_REG_PID, (uint8_t *)pid, 4);
    pid[4] = '\0';
}

/**
 * @brief       ATK-MD0430???
 * @param       
 * @retval      ATK_MD0430_TOUCH_EOK  : ATK-MD0430????
 *              ATK_MD0430_TOUCH_ERROR: ATK-MD0430????
 */
uint8_t atk_md0430_touch_init(void)
{
    char pid[5];
    
    atk_md0430_touch_hw_init();
    atk_md0430_touch_hw_reset(ATK_MD0430_TOUCH_IIC_ADDR_14);
    atk_md0430_touch_iic_init(ATK_MD0430_TOUCH_IIC_ADDR_14);
    delay_ms(100);
    atk_md0430_touch_get_pid(pid);
    if ((strcmp(pid, ATK_MD0430_TOUCH_PID1) != 0) && (strcmp(pid, ATK_MD0430_TOUCH_PID2) != 0))
    {
        return ATK_MD0430_TOUCH_ERROR;
    }
    atk_md0430_touch_sw_reset();
    
    return ATK_MD0430_TOUCH_EOK;
}

/**
 * @brief       ATK-MD0430???
 * @note        ü10ms
 * @param       point: ????
 *              cnt  : ???1~ATK_MD0430_TOUCH_TP_MAX
 * @retval      0   : û??
 *              : ?????
 */
uint8_t atk_md0430_touch_scan(atk_md0430_touch_point_t *point, uint8_t cnt)
{
    uint8_t tp_info;
    uint8_t tp_cnt;
    uint8_t point_index;
    atk_md0430_lcd_disp_dir_t dir;
    uint8_t tpn_info[6];
    atk_md0430_touch_point_t point_raw;
    
    if ((cnt == 0) || (cnt > ATK_MD0430_TOUCH_TP_MAX))
    {
        return 0;
    }
    
    for (point_index=0; point_index<cnt; point_index++)
    {
        if (&point[point_index] == NULL)
        {
            return 0;
        }
    }
    
    atk_md0430_touch_iic_read_reg(ATK_MD0430_TOUCH_REG_TPINFO, &tp_info, sizeof(tp_info));
    if ((tp_info & ATK_MD0430_TOUCH_TPINFO_MASK_STA) == ATK_MD0430_TOUCH_TPINFO_MASK_STA)
    {
        tp_cnt = tp_info & ATK_MD0430_TOUCH_TPINFO_MASK_CNT;
        tp_cnt = (cnt < tp_cnt) ? cnt : tp_cnt;
        
        for (point_index=0; point_index<tp_cnt; point_index++)
        {
            atk_md0430_touch_iic_read_reg(g_atk_md0430_touch_tp_reg[point_index], tpn_info, sizeof(tpn_info));
            point_raw.x = (uint16_t)(tpn_info[1] << 8) | tpn_info[0];
            point_raw.y = (uint16_t)(tpn_info[3] << 8) | tpn_info[2];
            point_raw.size = (uint16_t)(tpn_info[5] << 8) | tpn_info[4];
            
            dir = atk_md0430_get_disp_dir();
            switch (dir)
            {
                case ATK_MD0430_LCD_DISP_DIR_0:
                {
                    point[point_index].x = point_raw.x;
                    point[point_index].y = point_raw.y;
                    break;
                }
                case ATK_MD0430_LCD_DISP_DIR_90:
                {
                    point[point_index].x = point_raw.y;
                    point[point_index].y = atk_md0430_get_lcd_height() - point_raw.x;
                    break;
                }
                case ATK_MD0430_LCD_DISP_DIR_180:
                {
                    point[point_index].x = atk_md0430_get_lcd_width() - point_raw.x;
                    point[point_index].y = atk_md0430_get_lcd_height() - point_raw.y;
                    break;
                }
                case ATK_MD0430_LCD_DISP_DIR_270:
                {
                    point[point_index].x = atk_md0430_get_lcd_width() - point_raw.y;
                    point[point_index].y = point_raw.x;
                    break;
                }
            }
            
            point[point_index].size = point_raw.size;
        }
        
        tp_info = 0;
        atk_md0430_touch_iic_write_reg(ATK_MD0430_TOUCH_REG_TPINFO, &tp_info, sizeof(tp_info));
        
        return tp_cnt;
    }
    else
    {
        return 0;
    }
}
