#ifndef __ATK_MD0430_H__
#define __ATK_MD0430_H__

#include "FSMC_Class.h"
#include "GPIO_Class.h"
#include "I2C_Software_Class.h"
#include "ATK_MD0430_Font.h"

#include <string.h>

/* FSMC 相关配置 */
#define ATK_MD0430_FSMC_BANK            FSMC_NORSRAM_BANK4      // 使用 FSMC Bank4
#define ATK_MD0430_FSMC_BANK_ADDR       (0x6C000000)            // Bank4 基地址
#define ATK_MD0430_FSMC_REG_SEL         (6)                     // A6 引脚作为命令/数据切换位 (RS)

#define ATK_MD0430_TOUCH_IIC_ADDR_14     0x14
#define ATK_MD0430_TOUCH_IIC_ADDR_5D     0x5D

/* 屏幕物理尺寸 */
#define ATK_MD0430_LCD_WIDTH            480                     // 屏幕宽度
#define ATK_MD0430_LCD_HEIGHT           800                     // 屏幕高度

/* RGB565 颜色定义 */
#define ATK_MD0430_WHITE                0xFFFF
#define ATK_MD0430_BLACK                0x0000
#define ATK_MD0430_BLUE                 0x001F
#define ATK_MD0430_BRED                 0XF81F
#define ATK_MD0430_GRED                 0XFFE0
#define ATK_MD0430_GBLUE                0X07FF
#define ATK_MD0430_RED                  0xF800
#define ATK_MD0430_MAGENTA              0xF81F
#define ATK_MD0430_GREEN                0x07E0
#define ATK_MD0430_CYAN                 0x7FFF
#define ATK_MD0430_YELLOW               0xFFE0
#define ATK_MD0430_BROWN                0XBC40
#define ATK_MD0430_BRRED                0XFC07
#define ATK_MD0430_GRAY                 0X8430

/* 错误码定义 */
#define ATK_MD0430_EOK                  0   // 无错误
#define ATK_MD0430_ERROR                1   // 通用错误
#define ATK_MD0430_EINVAL               2   // 参数非法

/* 兼容的 LCD 驱动芯片 ID */
#define ATK_MD0430_CHIP_ID1             0x5510
#define ATK_MD0430_CHIP_ID2             0x9806

/* 触摸功能宏定义 */
#define ATK_MD0430_TOUCH_EOK            0   // 正常
#define ATK_MD0430_TOUCH_ERROR          1   // 触摸芯片响应错误
#define ATK_MD0430_TOUCH_PID1           "911"
#define ATK_MD0430_TOUCH_PID2           "1158"
#define ATK_MD0430_TOUCH_TP_MAX         5   // 最大支持5点触控

/* 触摸芯片寄存器地址 */
#define ATK_MD0430_TOUCH_REG_CTRL       0x8040  // 控制寄存器
#define ATK_MD0430_TOUCH_REG_PID        0x8140  // 产品 ID 寄存器
#define ATK_MD0430_TOUCH_REG_TPINFO     0x814E  // 触摸状态寄存器
#define ATK_MD0430_TOUCH_REG_TP1        0x8150  // 触点1数据寄存器
#define ATK_MD0430_TOUCH_REG_TP2        0x8158  // 触点2数据寄存器
#define ATK_MD0430_TOUCH_REG_TP3        0x8160  // 触点3数据寄存器
#define ATK_MD0430_TOUCH_REG_TP4        0x8168  // 触点4数据寄存器
#define ATK_MD0430_TOUCH_REG_TP5        0x8170  // 触点5数据寄存器

#define ATK_MD0430_TOUCH_TPINFO_MASK_STA    0x80 // 触摸就绪掩码
#define ATK_MD0430_TOUCH_TPINFO_MASK_CNT    0x0F // 触摸点数掩码

/* LCD 扫描方向定义（显存写入顺序） */
typedef enum
{
    ATK_MD0430_LCD_SCAN_DIR_L2R_U2D = 0x0000,   // 从左到右，从上到下
    ATK_MD0430_LCD_SCAN_DIR_L2R_D2U = 0x0080,   // 从左到右，从下到上
    ATK_MD0430_LCD_SCAN_DIR_R2L_U2D = 0x0040,   // 从右到左，从上到下
    ATK_MD0430_LCD_SCAN_DIR_R2L_D2U = 0x00C0,   // 从右到左，从下到上
    ATK_MD0430_LCD_SCAN_DIR_U2D_L2R = 0x0020,   // 从上到下，从左到右
    ATK_MD0430_LCD_SCAN_DIR_U2D_R2L = 0x0060,   // 从上到下，从右到左
    ATK_MD0430_LCD_SCAN_DIR_D2U_L2R = 0x00A0,   // 从下到上，从左到右
    ATK_MD0430_LCD_SCAN_DIR_D2U_R2L = 0x00E0,   // 从下到上，从右到左
} atk_md0430_lcd_scan_dir_t;

/* LCD 屏幕显示方向（旋转角度） */
typedef enum
{
    ATK_MD0430_LCD_DISP_DIR_0 = 0x00,           // 竖屏方向
    ATK_MD0430_LCD_DISP_DIR_90,                 // 顺时针旋转90度（横屏）
    ATK_MD0430_LCD_DISP_DIR_180,                // 顺时针旋转180度
    ATK_MD0430_LCD_DISP_DIR_270,                // 顺时针旋转270度
} atk_md0430_lcd_disp_dir_t;

/* 字体大小枚举 */
typedef enum
{
    ATK_MD0430_LCD_FONT_12,
    ATK_MD0430_LCD_FONT_16,
    ATK_MD0430_LCD_FONT_24,
    ATK_MD0430_LCD_FONT_32,
} atk_md0430_lcd_font_t;

/* 数字显示模式 */
typedef enum
{
    ATK_MD0430_NUM_SHOW_NOZERO = 0x00,  // 高位不显示0
    ATK_MD0430_NUM_SHOW_ZERO,           // 高位补0显示
} atk_md0430_num_mode_t;

/* LCD 状态结构体 */
typedef struct
{
    uint16_t chip_id;                   // 芯片 ID
    uint16_t width;                     // 当前显示宽度
    uint16_t height;                    // 当前显示高度
    atk_md0430_lcd_scan_dir_t scan_dir; // 当前扫描方向
    atk_md0430_lcd_disp_dir_t disp_dir; // 当前显示方向
} g_atk_md0430_sta;

/* 触摸坐标点结构体 */
typedef struct
{
    uint16_t x;     // X 坐标
    uint16_t y;     // Y 坐标
    uint16_t size;  // 触摸点大小（或压力）
} atk_md0430_touch_point_t;

/* ATK_MD0430 驱动类 */
class ATK_MD0430
{
private:
    MCU_FSMC lcd_fsmc;                  // FSMC 接口实例
    I2C_Software touch_i2c;                  // 触摸 IIC 接口实例
    MCU_GPIO LCD_BL;                    // 背光控制引脚
    MCU_GPIO TOUCH_TCS;                 // 触摸片选/使能引脚
    MCU_GPIO TOUCH_PEN;                 // 触摸中断引脚

    g_atk_md0430_sta State;             // 屏幕状态
    atk_md0430_touch_point_t Touch_Point; // 存储触摸数据

    const uint16_t touch_tp_reg[ATK_MD0430_TOUCH_TP_MAX] = {
        ATK_MD0430_TOUCH_REG_TP1,
        ATK_MD0430_TOUCH_REG_TP2,
        ATK_MD0430_TOUCH_REG_TP3,
        ATK_MD0430_TOUCH_REG_TP4,
        ATK_MD0430_TOUCH_REG_TP5,
    };

private:
    uint16_t get_chip_id(void);         // 读取驱动芯片 ID
    void set_column_address(uint16_t sc, uint16_t ec); // 设置列地址范围
    void set_page_address(uint16_t sp, uint16_t ep);   // 设置行地址范围
    void start_write_memory(void);      // 开始写显存指令
    void start_read_memory(void);       // 开始读显存指令
    uint32_t pow(uint8_t x, uint8_t y); // 简单幂运算

    void touch_hw_reset(uint8_t addr);
    void touch_sw_reset(void);
    void touch_get_pid(char *pid);

public:
    // 构造函数：初始化外设接口
    ATK_MD0430( MCU_GPIO LCD_BL = MCU_GPIO(GPIOB, GPIO_PIN_15), MCU_GPIO TOUCH_TCS = MCU_GPIO(GPIOC, GPIO_PIN_13), MCU_GPIO TOUCH_PEN = MCU_GPIO(GPIOB, GPIO_PIN_1),
                I2C_Software touch_i2c = I2C_Software(MCU_GPIO(GPIOB, GPIO_PIN_0), MCU_GPIO(GPIOF, GPIO_PIN_11), ATK_MD0430_TOUCH_IIC_ADDR_14),
                MCU_FSMC lcd_fsmc = MCU_FSMC(ATK_MD0430_FSMC_BANK_ADDR, ATK_MD0430_FSMC_REG_SEL));
    ~ATK_MD0430();

    uint8_t init(void);                 // LCD 初始化主入口
    void reg_init(void);                // 驱动芯片寄存器配置内容

    /* 属性获取内联函数 */
    inline uint16_t get_lcd_width(void) { return State.width; }
    inline uint16_t get_lcd_height(void) { return State.height; }
    inline atk_md0430_lcd_scan_dir_t get_scan_dir(void) { return State.scan_dir; }
    inline atk_md0430_lcd_disp_dir_t get_disp_dir(void) { return State.disp_dir; }
    
    /* 背光控制 */
    inline void backlight_on(void) { LCD_BL.high(); }
    inline void backlight_off(void) { LCD_BL.low(); }

    /* 屏幕显示开关 */
    void display_on(void);              
    void display_off(void); 

    /* 方向设置 */
    uint8_t set_scan_dir(atk_md0430_lcd_scan_dir_t scan_dir); // 设置扫描方向
    uint8_t set_disp_dir(atk_md0430_lcd_disp_dir_t disp_dir); // 设置旋转方向

    /* 绘图函数 */
    void fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color); // 区域填充
    inline void clear(uint16_t color) { fill(0, 0, State.width - 1, State.height - 1, color); } // 全屏清屏
    void draw_point(uint16_t x, uint16_t y, uint16_t color); // 画点
    uint16_t read_point(uint16_t x, uint16_t y);            // 读点颜色
    void draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color); // 画线
    void draw_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color); // 画矩形
    void draw_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);               // 画圆

    /* 字符与文本显示 */
    void show_char(uint16_t x, uint16_t y, char ch, atk_md0430_lcd_font_t font, uint16_t color); // 显示字符
    void show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, atk_md0430_lcd_font_t font, uint16_t color); // 显示字符串
    void show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, atk_md0430_num_mode_t mode, atk_md0430_lcd_font_t font, uint16_t color); // 显示数字
    inline void show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, atk_md0430_lcd_font_t font, uint16_t color)
                         { show_xnum(x, y, num, len, ATK_MD0430_NUM_SHOW_NOZERO, font, color); };
    
    /* 图片显示 */
    void show_pic(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *pic); 

    /* 触摸屏控制 */
    uint8_t touch_init(void);           // 初始化触摸芯片
    uint8_t touch_scan(atk_md0430_touch_point_t *point, uint8_t cnt); // 扫描触摸数据
};

#endif