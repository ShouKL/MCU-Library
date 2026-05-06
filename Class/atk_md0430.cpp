#include "atk_md0430.h"
#include "atk_md0430_font.h"
#include "atk_md0430_fsmc.h"
#include "delay.h"

/* ATK-MD0430ģ��LCD������ID */
#define ATK_MD0430_CHIP_ID1         0x5510
#define ATK_MD0430_CHIP_ID2         0x9806

/* ATK-MD0430ģ��LCDɨ�跽�� */
#define ATK_MD0430_SCAN_DIR_L2R_U2D (0x0000)
#define ATK_MD0430_SCAN_DIR_L2R_D2U (0x0080)
#define ATK_MD0430_SCAN_DIR_R2L_U2D (0x0040)
#define ATK_MD0430_SCAN_DIR_R2L_D2U (0x00C0)
#define ATK_MD0430_SCAN_DIR_U2D_L2R (0x0020)
#define ATK_MD0430_SCAN_DIR_U2D_R2L (0x0060)
#define ATK_MD0430_SCAN_DIR_D2U_L2R (0x00A0)
#define ATK_MD0430_SCAN_DIR_D2U_R2L (0x00E0)

/* ATK-MD0430ģ��״̬���ݽṹ�� */
static struct
{
    uint16_t chip_id;                   /* ������ID */
    uint16_t width;                     /* LCD���� */
    uint16_t height;                    /* LCD�߶� */
    atk_md0430_lcd_scan_dir_t scan_dir; /* LCDɨ�跽�� */
    atk_md0430_lcd_disp_dir_t disp_dir; /* LCD��ʾ���� */
} g_atk_md0430_sta = {0};

/**
 * @brief       ATK-MD0430ģ��Ӳ����ʼ��
 * @param       ��
 * @retval      ��
 */
static void atk_md0430_hw_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* ʹ��ʱ�� */
    ATK_MD0430_BL_GPIO_CLK_ENABLE();
    
    /* ��ʼ��BL���� */
    gpio_init_struct.Pin    = ATK_MD0430_BL_GPIO_PIN;
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull   = GPIO_PULLUP;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ATK_MD0430_BL_GPIO_PORT, &gpio_init_struct);
    
    ATK_MD0430_BL(0);
}

/**
 * @brief       ��ȡATK-MD0430ģ��������ID
 * @param       ��
 * @retval      ��
 */
static inline uint16_t atk_md0430_get_chip_id(void)
{
    uint16_t chip_id;
    
    atk_md0430_fsmc_write_reg(0xF000, 0x0055);
    atk_md0430_fsmc_write_reg(0xF001, 0x00AA);
    atk_md0430_fsmc_write_reg(0xF002, 0x0052);
    atk_md0430_fsmc_write_reg(0xF003, 0x0008);
    atk_md0430_fsmc_write_reg(0xF004, 0x0001);
    
    atk_md0430_fsmc_write_cmd(0xC500);
    chip_id = atk_md0430_fsmc_read_dat() << 8;
    atk_md0430_fsmc_write_cmd(0xC501);
    chip_id |= (atk_md0430_fsmc_read_dat()) & 0x00FF;

    if (chip_id != ATK_MD0430_CHIP_ID1)
    {
        /* ���Ի�ȡID2 */
        atk_md0430_fsmc_write_cmd(0XD3);
        chip_id = atk_md0430_fsmc_read_dat();
        chip_id = atk_md0430_fsmc_read_dat();
        chip_id = atk_md0430_fsmc_read_dat() << 8;
        chip_id |= atk_md0430_fsmc_read_dat();
    }

    return chip_id;
}

/**
 * @brief       ATK-MD0430ģ��Ĵ�����ʼ��
 * @param       ��
 * @retval      ��
 */
static void atk_md0430_reg_init(void)
{
    if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID1)
    {
        atk_md0430_fsmc_write_reg(0xF000, 0x55);
        atk_md0430_fsmc_write_reg(0xF001, 0xAA);
        atk_md0430_fsmc_write_reg(0xF002, 0x52);
        atk_md0430_fsmc_write_reg(0xF003, 0x08);
        atk_md0430_fsmc_write_reg(0xF004, 0x01);
        atk_md0430_fsmc_write_reg(0xB000, 0x0D);
        atk_md0430_fsmc_write_reg(0xB001, 0x0D);
        atk_md0430_fsmc_write_reg(0xB002, 0x0D);
        atk_md0430_fsmc_write_reg(0xB600, 0x34);
        atk_md0430_fsmc_write_reg(0xB601, 0x34);
        atk_md0430_fsmc_write_reg(0xB602, 0x34);
        atk_md0430_fsmc_write_reg(0xB100, 0x0D);
        atk_md0430_fsmc_write_reg(0xB101, 0x0D);
        atk_md0430_fsmc_write_reg(0xB102, 0x0D);
        atk_md0430_fsmc_write_reg(0xB700, 0x34);
        atk_md0430_fsmc_write_reg(0xB701, 0x34);
        atk_md0430_fsmc_write_reg(0xB702, 0x34);
        atk_md0430_fsmc_write_reg(0xB200, 0x00);
        atk_md0430_fsmc_write_reg(0xB201, 0x00);
        atk_md0430_fsmc_write_reg(0xB202, 0x00);
        atk_md0430_fsmc_write_reg(0xB800, 0x24);
        atk_md0430_fsmc_write_reg(0xB801, 0x24);
        atk_md0430_fsmc_write_reg(0xB802, 0x24);
        atk_md0430_fsmc_write_reg(0xBF00, 0x01);
        atk_md0430_fsmc_write_reg(0xB300, 0x0F);
        atk_md0430_fsmc_write_reg(0xB301, 0x0F);
        atk_md0430_fsmc_write_reg(0xB302, 0x0F);
        atk_md0430_fsmc_write_reg(0xB900, 0x34);
        atk_md0430_fsmc_write_reg(0xB901, 0x34);
        atk_md0430_fsmc_write_reg(0xB902, 0x34);
        atk_md0430_fsmc_write_reg(0xB500, 0x08);
        atk_md0430_fsmc_write_reg(0xB501, 0x08);
        atk_md0430_fsmc_write_reg(0xB502, 0x08);
        atk_md0430_fsmc_write_reg(0xC200, 0x03);
        atk_md0430_fsmc_write_reg(0xBA00, 0x24);
        atk_md0430_fsmc_write_reg(0xBA01, 0x24);
        atk_md0430_fsmc_write_reg(0xBA02, 0x24);
        atk_md0430_fsmc_write_reg(0xBC00, 0x00);
        atk_md0430_fsmc_write_reg(0xBC01, 0x78);
        atk_md0430_fsmc_write_reg(0xBC02, 0x00);
        atk_md0430_fsmc_write_reg(0xBD00, 0x00);
        atk_md0430_fsmc_write_reg(0xBD01, 0x78);
        atk_md0430_fsmc_write_reg(0xBD02, 0x00);
        atk_md0430_fsmc_write_reg(0xBE00, 0x00);
        atk_md0430_fsmc_write_reg(0xBE01, 0x64);
        atk_md0430_fsmc_write_reg(0xD100, 0x00);
        atk_md0430_fsmc_write_reg(0xD101, 0x33);
        atk_md0430_fsmc_write_reg(0xD102, 0x00);
        atk_md0430_fsmc_write_reg(0xD103, 0x34);
        atk_md0430_fsmc_write_reg(0xD104, 0x00);
        atk_md0430_fsmc_write_reg(0xD105, 0x3A);
        atk_md0430_fsmc_write_reg(0xD106, 0x00);
        atk_md0430_fsmc_write_reg(0xD107, 0x4A);
        atk_md0430_fsmc_write_reg(0xD108, 0x00);
        atk_md0430_fsmc_write_reg(0xD109, 0x5C);
        atk_md0430_fsmc_write_reg(0xD10A, 0x00);
        atk_md0430_fsmc_write_reg(0xD10B, 0x81);
        atk_md0430_fsmc_write_reg(0xD10C, 0x00);
        atk_md0430_fsmc_write_reg(0xD10D, 0xA6);
        atk_md0430_fsmc_write_reg(0xD10E, 0x00);
        atk_md0430_fsmc_write_reg(0xD10F, 0xE5);
        atk_md0430_fsmc_write_reg(0xD110, 0x01);
        atk_md0430_fsmc_write_reg(0xD111, 0x13);
        atk_md0430_fsmc_write_reg(0xD112, 0x01);
        atk_md0430_fsmc_write_reg(0xD113, 0x54);
        atk_md0430_fsmc_write_reg(0xD114, 0x01);
        atk_md0430_fsmc_write_reg(0xD115, 0x82);
        atk_md0430_fsmc_write_reg(0xD116, 0x01);
        atk_md0430_fsmc_write_reg(0xD117, 0xCA);
        atk_md0430_fsmc_write_reg(0xD118, 0x02);
        atk_md0430_fsmc_write_reg(0xD119, 0x00);
        atk_md0430_fsmc_write_reg(0xD11A, 0x02);
        atk_md0430_fsmc_write_reg(0xD11B, 0x01);
        atk_md0430_fsmc_write_reg(0xD11C, 0x02);
        atk_md0430_fsmc_write_reg(0xD11D, 0x34);
        atk_md0430_fsmc_write_reg(0xD11E, 0x02);
        atk_md0430_fsmc_write_reg(0xD11F, 0x67);
        atk_md0430_fsmc_write_reg(0xD120, 0x02);
        atk_md0430_fsmc_write_reg(0xD121, 0x84);
        atk_md0430_fsmc_write_reg(0xD122, 0x02);
        atk_md0430_fsmc_write_reg(0xD123, 0xA4);
        atk_md0430_fsmc_write_reg(0xD124, 0x02);
        atk_md0430_fsmc_write_reg(0xD125, 0xB7);
        atk_md0430_fsmc_write_reg(0xD126, 0x02);
        atk_md0430_fsmc_write_reg(0xD127, 0xCF);
        atk_md0430_fsmc_write_reg(0xD128, 0x02);
        atk_md0430_fsmc_write_reg(0xD129, 0xDE);
        atk_md0430_fsmc_write_reg(0xD12A, 0x02);
        atk_md0430_fsmc_write_reg(0xD12B, 0xF2);
        atk_md0430_fsmc_write_reg(0xD12C, 0x02);
        atk_md0430_fsmc_write_reg(0xD12D, 0xFE);
        atk_md0430_fsmc_write_reg(0xD12E, 0x03);
        atk_md0430_fsmc_write_reg(0xD12F, 0x10);
        atk_md0430_fsmc_write_reg(0xD130, 0x03);
        atk_md0430_fsmc_write_reg(0xD131, 0x33);
        atk_md0430_fsmc_write_reg(0xD132, 0x03);
        atk_md0430_fsmc_write_reg(0xD133, 0x6D);
        atk_md0430_fsmc_write_reg(0xD200, 0x00);
        atk_md0430_fsmc_write_reg(0xD201, 0x33);
        atk_md0430_fsmc_write_reg(0xD202, 0x00);
        atk_md0430_fsmc_write_reg(0xD203, 0x34);
        atk_md0430_fsmc_write_reg(0xD204, 0x00);
        atk_md0430_fsmc_write_reg(0xD205, 0x3A);
        atk_md0430_fsmc_write_reg(0xD206, 0x00);
        atk_md0430_fsmc_write_reg(0xD207, 0x4A);
        atk_md0430_fsmc_write_reg(0xD208, 0x00);
        atk_md0430_fsmc_write_reg(0xD209, 0x5C);
        atk_md0430_fsmc_write_reg(0xD20A, 0x00);
        atk_md0430_fsmc_write_reg(0xD20B, 0x81);
        atk_md0430_fsmc_write_reg(0xD20C, 0x00);
        atk_md0430_fsmc_write_reg(0xD20D, 0xA6);
        atk_md0430_fsmc_write_reg(0xD20E, 0x00);
        atk_md0430_fsmc_write_reg(0xD20F, 0xE5);
        atk_md0430_fsmc_write_reg(0xD210, 0x01);
        atk_md0430_fsmc_write_reg(0xD211, 0x13);
        atk_md0430_fsmc_write_reg(0xD212, 0x01);
        atk_md0430_fsmc_write_reg(0xD213, 0x54);
        atk_md0430_fsmc_write_reg(0xD214, 0x01);
        atk_md0430_fsmc_write_reg(0xD215, 0x82);
        atk_md0430_fsmc_write_reg(0xD216, 0x01);
        atk_md0430_fsmc_write_reg(0xD217, 0xCA);
        atk_md0430_fsmc_write_reg(0xD218, 0x02);
        atk_md0430_fsmc_write_reg(0xD219, 0x00);
        atk_md0430_fsmc_write_reg(0xD21A, 0x02);
        atk_md0430_fsmc_write_reg(0xD21B, 0x01);
        atk_md0430_fsmc_write_reg(0xD21C, 0x02);
        atk_md0430_fsmc_write_reg(0xD21D, 0x34);
        atk_md0430_fsmc_write_reg(0xD21E, 0x02);
        atk_md0430_fsmc_write_reg(0xD21F, 0x67);
        atk_md0430_fsmc_write_reg(0xD220, 0x02);
        atk_md0430_fsmc_write_reg(0xD221, 0x84);
        atk_md0430_fsmc_write_reg(0xD222, 0x02);
        atk_md0430_fsmc_write_reg(0xD223, 0xA4);
        atk_md0430_fsmc_write_reg(0xD224, 0x02);
        atk_md0430_fsmc_write_reg(0xD225, 0xB7);
        atk_md0430_fsmc_write_reg(0xD226, 0x02);
        atk_md0430_fsmc_write_reg(0xD227, 0xCF);
        atk_md0430_fsmc_write_reg(0xD228, 0x02);
        atk_md0430_fsmc_write_reg(0xD229, 0xDE);
        atk_md0430_fsmc_write_reg(0xD22A, 0x02);
        atk_md0430_fsmc_write_reg(0xD22B, 0xF2);
        atk_md0430_fsmc_write_reg(0xD22C, 0x02);
        atk_md0430_fsmc_write_reg(0xD22D, 0xFE);
        atk_md0430_fsmc_write_reg(0xD22E, 0x03);
        atk_md0430_fsmc_write_reg(0xD22F, 0x10);
        atk_md0430_fsmc_write_reg(0xD230, 0x03);
        atk_md0430_fsmc_write_reg(0xD231, 0x33);
        atk_md0430_fsmc_write_reg(0xD232, 0x03);
        atk_md0430_fsmc_write_reg(0xD233, 0x6D);
        atk_md0430_fsmc_write_reg(0xD300, 0x00);
        atk_md0430_fsmc_write_reg(0xD301, 0x33);
        atk_md0430_fsmc_write_reg(0xD302, 0x00);
        atk_md0430_fsmc_write_reg(0xD303, 0x34);
        atk_md0430_fsmc_write_reg(0xD304, 0x00);
        atk_md0430_fsmc_write_reg(0xD305, 0x3A);
        atk_md0430_fsmc_write_reg(0xD306, 0x00);
        atk_md0430_fsmc_write_reg(0xD307, 0x4A);
        atk_md0430_fsmc_write_reg(0xD308, 0x00);
        atk_md0430_fsmc_write_reg(0xD309, 0x5C);
        atk_md0430_fsmc_write_reg(0xD30A, 0x00);
        atk_md0430_fsmc_write_reg(0xD30B, 0x81);
        atk_md0430_fsmc_write_reg(0xD30C, 0x00);
        atk_md0430_fsmc_write_reg(0xD30D, 0xA6);
        atk_md0430_fsmc_write_reg(0xD30E, 0x00);
        atk_md0430_fsmc_write_reg(0xD30F, 0xE5);
        atk_md0430_fsmc_write_reg(0xD310, 0x01);
        atk_md0430_fsmc_write_reg(0xD311, 0x13);
        atk_md0430_fsmc_write_reg(0xD312, 0x01);
        atk_md0430_fsmc_write_reg(0xD313, 0x54);
        atk_md0430_fsmc_write_reg(0xD314, 0x01);
        atk_md0430_fsmc_write_reg(0xD315, 0x82);
        atk_md0430_fsmc_write_reg(0xD316, 0x01);
        atk_md0430_fsmc_write_reg(0xD317, 0xCA);
        atk_md0430_fsmc_write_reg(0xD318, 0x02);
        atk_md0430_fsmc_write_reg(0xD319, 0x00);
        atk_md0430_fsmc_write_reg(0xD31A, 0x02);
        atk_md0430_fsmc_write_reg(0xD31B, 0x01);
        atk_md0430_fsmc_write_reg(0xD31C, 0x02);
        atk_md0430_fsmc_write_reg(0xD31D, 0x34);
        atk_md0430_fsmc_write_reg(0xD31E, 0x02);
        atk_md0430_fsmc_write_reg(0xD31F, 0x67);
        atk_md0430_fsmc_write_reg(0xD320, 0x02);
        atk_md0430_fsmc_write_reg(0xD321, 0x84);
        atk_md0430_fsmc_write_reg(0xD322, 0x02);
        atk_md0430_fsmc_write_reg(0xD323, 0xA4);
        atk_md0430_fsmc_write_reg(0xD324, 0x02);
        atk_md0430_fsmc_write_reg(0xD325, 0xB7);
        atk_md0430_fsmc_write_reg(0xD326, 0x02);
        atk_md0430_fsmc_write_reg(0xD327, 0xCF);
        atk_md0430_fsmc_write_reg(0xD328, 0x02);
        atk_md0430_fsmc_write_reg(0xD329, 0xDE);
        atk_md0430_fsmc_write_reg(0xD32A, 0x02);
        atk_md0430_fsmc_write_reg(0xD32B, 0xF2);
        atk_md0430_fsmc_write_reg(0xD32C, 0x02);
        atk_md0430_fsmc_write_reg(0xD32D, 0xFE);
        atk_md0430_fsmc_write_reg(0xD32E, 0x03);
        atk_md0430_fsmc_write_reg(0xD32F, 0x10);
        atk_md0430_fsmc_write_reg(0xD330, 0x03);
        atk_md0430_fsmc_write_reg(0xD331, 0x33);
        atk_md0430_fsmc_write_reg(0xD332, 0x03);
        atk_md0430_fsmc_write_reg(0xD333, 0x6D);
        atk_md0430_fsmc_write_reg(0xD400, 0x00);
        atk_md0430_fsmc_write_reg(0xD401, 0x33);
        atk_md0430_fsmc_write_reg(0xD402, 0x00);
        atk_md0430_fsmc_write_reg(0xD403, 0x34);
        atk_md0430_fsmc_write_reg(0xD404, 0x00);
        atk_md0430_fsmc_write_reg(0xD405, 0x3A);
        atk_md0430_fsmc_write_reg(0xD406, 0x00);
        atk_md0430_fsmc_write_reg(0xD407, 0x4A);
        atk_md0430_fsmc_write_reg(0xD408, 0x00);
        atk_md0430_fsmc_write_reg(0xD409, 0x5C);
        atk_md0430_fsmc_write_reg(0xD40A, 0x00);
        atk_md0430_fsmc_write_reg(0xD40B, 0x81);
        atk_md0430_fsmc_write_reg(0xD40C, 0x00);
        atk_md0430_fsmc_write_reg(0xD40D, 0xA6);
        atk_md0430_fsmc_write_reg(0xD40E, 0x00);
        atk_md0430_fsmc_write_reg(0xD40F, 0xE5);
        atk_md0430_fsmc_write_reg(0xD410, 0x01);
        atk_md0430_fsmc_write_reg(0xD411, 0x13);
        atk_md0430_fsmc_write_reg(0xD412, 0x01);
        atk_md0430_fsmc_write_reg(0xD413, 0x54);
        atk_md0430_fsmc_write_reg(0xD414, 0x01);
        atk_md0430_fsmc_write_reg(0xD415, 0x82);
        atk_md0430_fsmc_write_reg(0xD416, 0x01);
        atk_md0430_fsmc_write_reg(0xD417, 0xCA);
        atk_md0430_fsmc_write_reg(0xD418, 0x02);
        atk_md0430_fsmc_write_reg(0xD419, 0x00);
        atk_md0430_fsmc_write_reg(0xD41A, 0x02);
        atk_md0430_fsmc_write_reg(0xD41B, 0x01);
        atk_md0430_fsmc_write_reg(0xD41C, 0x02);
        atk_md0430_fsmc_write_reg(0xD41D, 0x34);
        atk_md0430_fsmc_write_reg(0xD41E, 0x02);
        atk_md0430_fsmc_write_reg(0xD41F, 0x67);
        atk_md0430_fsmc_write_reg(0xD420, 0x02);
        atk_md0430_fsmc_write_reg(0xD421, 0x84);
        atk_md0430_fsmc_write_reg(0xD422, 0x02);
        atk_md0430_fsmc_write_reg(0xD423, 0xA4);
        atk_md0430_fsmc_write_reg(0xD424, 0x02);
        atk_md0430_fsmc_write_reg(0xD425, 0xB7);
        atk_md0430_fsmc_write_reg(0xD426, 0x02);
        atk_md0430_fsmc_write_reg(0xD427, 0xCF);
        atk_md0430_fsmc_write_reg(0xD428, 0x02);
        atk_md0430_fsmc_write_reg(0xD429, 0xDE);
        atk_md0430_fsmc_write_reg(0xD42A, 0x02);
        atk_md0430_fsmc_write_reg(0xD42B, 0xF2);
        atk_md0430_fsmc_write_reg(0xD42C, 0x02);
        atk_md0430_fsmc_write_reg(0xD42D, 0xFE);
        atk_md0430_fsmc_write_reg(0xD42E, 0x03);
        atk_md0430_fsmc_write_reg(0xD42F, 0x10);
        atk_md0430_fsmc_write_reg(0xD430, 0x03);
        atk_md0430_fsmc_write_reg(0xD431, 0x33);
        atk_md0430_fsmc_write_reg(0xD432, 0x03);
        atk_md0430_fsmc_write_reg(0xD433, 0x6D);
        atk_md0430_fsmc_write_reg(0xD500, 0x00);
        atk_md0430_fsmc_write_reg(0xD501, 0x33);
        atk_md0430_fsmc_write_reg(0xD502, 0x00);
        atk_md0430_fsmc_write_reg(0xD503, 0x34);
        atk_md0430_fsmc_write_reg(0xD504, 0x00);
        atk_md0430_fsmc_write_reg(0xD505, 0x3A);
        atk_md0430_fsmc_write_reg(0xD506, 0x00);
        atk_md0430_fsmc_write_reg(0xD507, 0x4A);
        atk_md0430_fsmc_write_reg(0xD508, 0x00);
        atk_md0430_fsmc_write_reg(0xD509, 0x5C);
        atk_md0430_fsmc_write_reg(0xD50A, 0x00);
        atk_md0430_fsmc_write_reg(0xD50B, 0x81);
        atk_md0430_fsmc_write_reg(0xD50C, 0x00);
        atk_md0430_fsmc_write_reg(0xD50D, 0xA6);
        atk_md0430_fsmc_write_reg(0xD50E, 0x00);
        atk_md0430_fsmc_write_reg(0xD50F, 0xE5);
        atk_md0430_fsmc_write_reg(0xD510, 0x01);
        atk_md0430_fsmc_write_reg(0xD511, 0x13);
        atk_md0430_fsmc_write_reg(0xD512, 0x01);
        atk_md0430_fsmc_write_reg(0xD513, 0x54);
        atk_md0430_fsmc_write_reg(0xD514, 0x01);
        atk_md0430_fsmc_write_reg(0xD515, 0x82);
        atk_md0430_fsmc_write_reg(0xD516, 0x01);
        atk_md0430_fsmc_write_reg(0xD517, 0xCA);
        atk_md0430_fsmc_write_reg(0xD518, 0x02);
        atk_md0430_fsmc_write_reg(0xD519, 0x00);
        atk_md0430_fsmc_write_reg(0xD51A, 0x02);
        atk_md0430_fsmc_write_reg(0xD51B, 0x01);
        atk_md0430_fsmc_write_reg(0xD51C, 0x02);
        atk_md0430_fsmc_write_reg(0xD51D, 0x34);
        atk_md0430_fsmc_write_reg(0xD51E, 0x02);
        atk_md0430_fsmc_write_reg(0xD51F, 0x67);
        atk_md0430_fsmc_write_reg(0xD520, 0x02);
        atk_md0430_fsmc_write_reg(0xD521, 0x84);
        atk_md0430_fsmc_write_reg(0xD522, 0x02);
        atk_md0430_fsmc_write_reg(0xD523, 0xA4);
        atk_md0430_fsmc_write_reg(0xD524, 0x02);
        atk_md0430_fsmc_write_reg(0xD525, 0xB7);
        atk_md0430_fsmc_write_reg(0xD526, 0x02);
        atk_md0430_fsmc_write_reg(0xD527, 0xCF);
        atk_md0430_fsmc_write_reg(0xD528, 0x02);
        atk_md0430_fsmc_write_reg(0xD529, 0xDE);
        atk_md0430_fsmc_write_reg(0xD52A, 0x02);
        atk_md0430_fsmc_write_reg(0xD52B, 0xF2);
        atk_md0430_fsmc_write_reg(0xD52C, 0x02);
        atk_md0430_fsmc_write_reg(0xD52D, 0xFE);
        atk_md0430_fsmc_write_reg(0xD52E, 0x03);
        atk_md0430_fsmc_write_reg(0xD52F, 0x10);
        atk_md0430_fsmc_write_reg(0xD530, 0x03);
        atk_md0430_fsmc_write_reg(0xD531, 0x33);
        atk_md0430_fsmc_write_reg(0xD532, 0x03);
        atk_md0430_fsmc_write_reg(0xD533, 0x6D);
        atk_md0430_fsmc_write_reg(0xD600, 0x00);
        atk_md0430_fsmc_write_reg(0xD601, 0x33);
        atk_md0430_fsmc_write_reg(0xD602, 0x00);
        atk_md0430_fsmc_write_reg(0xD603, 0x34);
        atk_md0430_fsmc_write_reg(0xD604, 0x00);
        atk_md0430_fsmc_write_reg(0xD605, 0x3A);
        atk_md0430_fsmc_write_reg(0xD606, 0x00);
        atk_md0430_fsmc_write_reg(0xD607, 0x4A);
        atk_md0430_fsmc_write_reg(0xD608, 0x00);
        atk_md0430_fsmc_write_reg(0xD609, 0x5C);
        atk_md0430_fsmc_write_reg(0xD60A, 0x00);
        atk_md0430_fsmc_write_reg(0xD60B, 0x81);
        atk_md0430_fsmc_write_reg(0xD60C, 0x00);
        atk_md0430_fsmc_write_reg(0xD60D, 0xA6);
        atk_md0430_fsmc_write_reg(0xD60E, 0x00);
        atk_md0430_fsmc_write_reg(0xD60F, 0xE5);
        atk_md0430_fsmc_write_reg(0xD610, 0x01);
        atk_md0430_fsmc_write_reg(0xD611, 0x13);
        atk_md0430_fsmc_write_reg(0xD612, 0x01);
        atk_md0430_fsmc_write_reg(0xD613, 0x54);
        atk_md0430_fsmc_write_reg(0xD614, 0x01);
        atk_md0430_fsmc_write_reg(0xD615, 0x82);
        atk_md0430_fsmc_write_reg(0xD616, 0x01);
        atk_md0430_fsmc_write_reg(0xD617, 0xCA);
        atk_md0430_fsmc_write_reg(0xD618, 0x02);
        atk_md0430_fsmc_write_reg(0xD619, 0x00);
        atk_md0430_fsmc_write_reg(0xD61A, 0x02);
        atk_md0430_fsmc_write_reg(0xD61B, 0x01);
        atk_md0430_fsmc_write_reg(0xD61C, 0x02);
        atk_md0430_fsmc_write_reg(0xD61D, 0x34);
        atk_md0430_fsmc_write_reg(0xD61E, 0x02);
        atk_md0430_fsmc_write_reg(0xD61F, 0x67);
        atk_md0430_fsmc_write_reg(0xD620, 0x02);
        atk_md0430_fsmc_write_reg(0xD621, 0x84);
        atk_md0430_fsmc_write_reg(0xD622, 0x02);
        atk_md0430_fsmc_write_reg(0xD623, 0xA4);
        atk_md0430_fsmc_write_reg(0xD624, 0x02);
        atk_md0430_fsmc_write_reg(0xD625, 0xB7);
        atk_md0430_fsmc_write_reg(0xD626, 0x02);
        atk_md0430_fsmc_write_reg(0xD627, 0xCF);
        atk_md0430_fsmc_write_reg(0xD628, 0x02);
        atk_md0430_fsmc_write_reg(0xD629, 0xDE);
        atk_md0430_fsmc_write_reg(0xD62A, 0x02);
        atk_md0430_fsmc_write_reg(0xD62B, 0xF2);
        atk_md0430_fsmc_write_reg(0xD62C, 0x02);
        atk_md0430_fsmc_write_reg(0xD62D, 0xFE);
        atk_md0430_fsmc_write_reg(0xD62E, 0x03);
        atk_md0430_fsmc_write_reg(0xD62F, 0x10);
        atk_md0430_fsmc_write_reg(0xD630, 0x03);
        atk_md0430_fsmc_write_reg(0xD631, 0x33);
        atk_md0430_fsmc_write_reg(0xD632, 0x03);
        atk_md0430_fsmc_write_reg(0xD633, 0x6D);
        atk_md0430_fsmc_write_reg(0xF000, 0x55);
        atk_md0430_fsmc_write_reg(0xF001, 0xAA);
        atk_md0430_fsmc_write_reg(0xF002, 0x52);
        atk_md0430_fsmc_write_reg(0xF003, 0x08);
        atk_md0430_fsmc_write_reg(0xF004, 0x00);
        atk_md0430_fsmc_write_reg(0xB100, 0xCC);
        atk_md0430_fsmc_write_reg(0xB101, 0x00);
        atk_md0430_fsmc_write_reg(0xB600, 0x05);
        atk_md0430_fsmc_write_reg(0xB700, 0x70);
        atk_md0430_fsmc_write_reg(0xB701, 0x70);
        atk_md0430_fsmc_write_reg(0xB800, 0x01);
        atk_md0430_fsmc_write_reg(0xB801, 0x03);
        atk_md0430_fsmc_write_reg(0xB802, 0x03);
        atk_md0430_fsmc_write_reg(0xB803, 0x03);
        atk_md0430_fsmc_write_reg(0xBC00, 0x02);
        atk_md0430_fsmc_write_reg(0xBC01, 0x00);
        atk_md0430_fsmc_write_reg(0xBC02, 0x00);
        atk_md0430_fsmc_write_reg(0xC900, 0xD0);
        atk_md0430_fsmc_write_reg(0xC901, 0x02);
        atk_md0430_fsmc_write_reg(0xC902, 0x50);
        atk_md0430_fsmc_write_reg(0xC903, 0x50);
        atk_md0430_fsmc_write_reg(0xC904, 0x50);
        atk_md0430_fsmc_write_reg(0x3500, 0x00);
        atk_md0430_fsmc_write_reg(0x3A00, 0x55);
        atk_md0430_fsmc_write_cmd(0x1100);
        delay_us(120);
        atk_md0430_fsmc_write_cmd(0x2900);
    }
    else if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID2)
    {
        atk_md0430_fsmc_write_cmd(0xFF); 
        atk_md0430_fsmc_write_dat(0xFF);
        atk_md0430_fsmc_write_dat(0x98);
        atk_md0430_fsmc_write_dat(0x06);
        atk_md0430_fsmc_write_cmd(0xBC);
        atk_md0430_fsmc_write_dat(0x01);
        atk_md0430_fsmc_write_dat(0x0F);
        atk_md0430_fsmc_write_dat(0x61);
        atk_md0430_fsmc_write_dat(0xFF);
        atk_md0430_fsmc_write_dat(0x01);
        atk_md0430_fsmc_write_dat(0x01);
        atk_md0430_fsmc_write_dat(0x0B);
        atk_md0430_fsmc_write_dat(0x10);
        atk_md0430_fsmc_write_dat(0x37);
        atk_md0430_fsmc_write_dat(0x63);
        atk_md0430_fsmc_write_dat(0xFF);
        atk_md0430_fsmc_write_dat(0xFF);
        atk_md0430_fsmc_write_dat(0x01);
        atk_md0430_fsmc_write_dat(0x01);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0xFF);
        atk_md0430_fsmc_write_dat(0x52);
        atk_md0430_fsmc_write_dat(0x01);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x40);
        atk_md0430_fsmc_write_cmd(0xBD);
        atk_md0430_fsmc_write_dat(0x01);
        atk_md0430_fsmc_write_dat(0x23);
        atk_md0430_fsmc_write_dat(0x45);
        atk_md0430_fsmc_write_dat(0x67);
        atk_md0430_fsmc_write_dat(0x01);
        atk_md0430_fsmc_write_dat(0x23);
        atk_md0430_fsmc_write_dat(0x45);
        atk_md0430_fsmc_write_dat(0x67);
        atk_md0430_fsmc_write_cmd(0xBE);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x01);
        atk_md0430_fsmc_write_dat(0xAB);
        atk_md0430_fsmc_write_dat(0x60);
        atk_md0430_fsmc_write_dat(0x22);
        atk_md0430_fsmc_write_dat(0x22);
        atk_md0430_fsmc_write_dat(0x22);
        atk_md0430_fsmc_write_dat(0x22);
        atk_md0430_fsmc_write_dat(0x22);
        atk_md0430_fsmc_write_cmd(0xC7);
        atk_md0430_fsmc_write_dat(0x36);
        atk_md0430_fsmc_write_cmd(0xED);
        atk_md0430_fsmc_write_dat(0x7F);
        atk_md0430_fsmc_write_dat(0x0F);
        atk_md0430_fsmc_write_cmd(0XC0);
        atk_md0430_fsmc_write_dat(0x0F);
        atk_md0430_fsmc_write_dat(0x0B);
        atk_md0430_fsmc_write_dat(0x0A);
        atk_md0430_fsmc_write_cmd(0XFC);
        atk_md0430_fsmc_write_dat(0x08);
        atk_md0430_fsmc_write_cmd(0XDF); 
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x20);
        atk_md0430_fsmc_write_cmd(0XF3);
        atk_md0430_fsmc_write_dat(0x74);
        atk_md0430_fsmc_write_cmd(0xB4);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_cmd(0xF7);
        atk_md0430_fsmc_write_dat(0x82);
        atk_md0430_fsmc_write_cmd(0xB1);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x13);
        atk_md0430_fsmc_write_dat(0x13); 
        atk_md0430_fsmc_write_cmd(0XF2);
        atk_md0430_fsmc_write_dat(0x80);
        atk_md0430_fsmc_write_dat(0x04);
        atk_md0430_fsmc_write_dat(0x40);
        atk_md0430_fsmc_write_dat(0x28);
        atk_md0430_fsmc_write_cmd(0XC1);
        atk_md0430_fsmc_write_dat(0x17);
        atk_md0430_fsmc_write_dat(0x88);
        atk_md0430_fsmc_write_dat(0x88);
        atk_md0430_fsmc_write_dat(0x20);
        atk_md0430_fsmc_write_cmd(0xE0);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x0A);
        atk_md0430_fsmc_write_dat(0x12);
        atk_md0430_fsmc_write_dat(0x10);
        atk_md0430_fsmc_write_dat(0x0E);
        atk_md0430_fsmc_write_dat(0x20);
        atk_md0430_fsmc_write_dat(0xCC);
        atk_md0430_fsmc_write_dat(0x07);
        atk_md0430_fsmc_write_dat(0x06);
        atk_md0430_fsmc_write_dat(0x0B);
        atk_md0430_fsmc_write_dat(0x0E);
        atk_md0430_fsmc_write_dat(0x0F);
        atk_md0430_fsmc_write_dat(0x0D);
        atk_md0430_fsmc_write_dat(0x15);
        atk_md0430_fsmc_write_dat(0x10);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_cmd(0xE1);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x0B);
        atk_md0430_fsmc_write_dat(0x13);
        atk_md0430_fsmc_write_dat(0x0D);
        atk_md0430_fsmc_write_dat(0x0E);
        atk_md0430_fsmc_write_dat(0x1B);
        atk_md0430_fsmc_write_dat(0x71);
        atk_md0430_fsmc_write_dat(0x06);
        atk_md0430_fsmc_write_dat(0x06);
        atk_md0430_fsmc_write_dat(0x0A);
        atk_md0430_fsmc_write_dat(0x0F);
        atk_md0430_fsmc_write_dat(0x0E);
        atk_md0430_fsmc_write_dat(0x0F);
        atk_md0430_fsmc_write_dat(0x15);
        atk_md0430_fsmc_write_dat(0x0C);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_cmd(0x2a);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x01);
        atk_md0430_fsmc_write_dat(0xdf);
        atk_md0430_fsmc_write_cmd(0x2b);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_dat(0x03);
        atk_md0430_fsmc_write_dat(0x1f);
        atk_md0430_fsmc_write_cmd(0x3A);
        atk_md0430_fsmc_write_dat(0x55);
        atk_md0430_fsmc_write_cmd(0x36);
        atk_md0430_fsmc_write_dat(0x00);
        atk_md0430_fsmc_write_cmd(0x11);
        delay_ms(120);   
        atk_md0430_fsmc_write_cmd(0x29);  
        delay_ms(20);  
        atk_md0430_fsmc_write_cmd(0x2C);
    }
}

/**
 * @brief       ����ATK-MD0430ģ���е�ַ
 * @param       ��
 * @retval      ��
 */
static void atk_md0430_set_column_address(uint16_t sc, uint16_t ec)
{
    if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID1)
    {
        atk_md0430_fsmc_write_cmd(0x2A00);
        atk_md0430_fsmc_write_dat((uint8_t)(sc >> 8) & 0xFF);
        atk_md0430_fsmc_write_cmd(0x2A01);
        atk_md0430_fsmc_write_dat((uint8_t)sc & 0xFF);
        atk_md0430_fsmc_write_cmd(0x2A02);
        atk_md0430_fsmc_write_dat((uint8_t)(ec >> 8) & 0xFF);
        atk_md0430_fsmc_write_cmd(0x2A03);
        atk_md0430_fsmc_write_dat((uint8_t)ec & 0xFF);
    }
    else if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID2)
    {
        atk_md0430_fsmc_write_cmd(0x2A);
        atk_md0430_fsmc_write_dat((uint8_t)(sc >> 8) & 0xFF);
        atk_md0430_fsmc_write_dat((uint8_t)sc & 0xFF);
        atk_md0430_fsmc_write_dat((uint8_t)(ec >> 8) & 0xFF);
        atk_md0430_fsmc_write_dat((uint8_t)ec & 0xFF);
    }
}

/**
 * @brief       ����ATK-MD0430ģ��ҳ��ַ
 * @param       ��
 * @retval      ��
 */
static void atk_md0430_set_page_address(uint16_t sp, uint16_t ep)
{
    if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID1)
    {
        atk_md0430_fsmc_write_cmd(0x2B00);
        atk_md0430_fsmc_write_dat((uint8_t)(sp >> 8) & 0xFF);
        atk_md0430_fsmc_write_cmd(0x2B01);
        atk_md0430_fsmc_write_dat((uint8_t)sp & 0xFF);
        atk_md0430_fsmc_write_cmd(0x2B02);
        atk_md0430_fsmc_write_dat((uint8_t)(ep >> 8) & 0xFF);
        atk_md0430_fsmc_write_cmd(0x2B03);
        atk_md0430_fsmc_write_dat((uint8_t)ep & 0xFF);
    }
    else if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID2)
    {
        atk_md0430_fsmc_write_cmd(0x2B);
        atk_md0430_fsmc_write_dat((uint8_t)(sp >> 8) & 0xFF);
        atk_md0430_fsmc_write_dat((uint8_t)sp & 0xFF);
        atk_md0430_fsmc_write_dat((uint8_t)(ep >> 8) & 0xFF);
        atk_md0430_fsmc_write_dat((uint8_t)ep & 0xFF);
    }
}

/**
 * @brief       ��ʼдATK-MD0430ģ���Դ�
 * @param       ��
 * @retval      ��
 */
static void atk_md0430_start_write_memory(void)
{
    if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID1)
    {
        atk_md0430_fsmc_write_cmd(0x2C00);
    }
    else if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID2)
    {
        atk_md0430_fsmc_write_cmd(0x2C);
    }
}

/**
 * @brief       ��ʼ��ATK-MD0430ģ���Դ�
 * @param       ��
 * @retval      ��
 */
static void atk_md0430_start_read_memory(void)
{
    if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID1)
    {
        atk_md0430_fsmc_write_cmd(0x2E00);
    }
    else if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID2)
    {
        atk_md0430_fsmc_write_cmd(0x2E);
    }
}

/**
 * @brief       ƽ��������x^y
 * @param       x: ����
 *              y: ָ��
 * @retval      x^y
 */
static uint32_t atk_md0430_pow(uint8_t x, uint8_t y)
{
    uint8_t loop;
    uint32_t res = 1;
    
    for (loop=0; loop<y; loop++)
    {
        res *= x;
    }
    
    return res;
}

/**
 * @brief       ATK-MD0430ģ���ʼ��
 * @param       ��
 * @retval      ATK_MD0430_EOK  : ATK_MD0430ģ���ʼ���ɹ�
 *              ATK_MD0430_ERROR: ATK_MD0430ģ���ʼ��ʧ��
 */
uint8_t atk_md0430_init(void)
{
    uint16_t chip_id;
#if (ATK_MD0430_USING_TOUCH != 0)
    uint8_t ret;
#endif
    
    atk_md0430_hw_init();               /* ATK-MD0430ģ��Ӳ����ʼ�� */
    atk_md0430_fsmc_init();             /* ATK-MD0430ģ��FSMC�ӿڳ�ʼ�� */
    chip_id = atk_md0430_get_chip_id(); /* ��ȡATK-MD0430ģ��������ID */
    if ((chip_id != ATK_MD0430_CHIP_ID1) && (chip_id != ATK_MD0430_CHIP_ID2))
    {
        return ATK_MD0430_ERROR;
    }
    else
    {
        g_atk_md0430_sta.chip_id = chip_id;
        g_atk_md0430_sta.width = ATK_MD0430_LCD_WIDTH;
        g_atk_md0430_sta.height = ATK_MD0430_LCD_HEIGHT;
    }
    atk_md0430_reg_init();
    atk_md0430_set_disp_dir(ATK_MD0430_LCD_DISP_DIR_0);
    atk_md0430_clear(ATK_MD0430_WHITE);
    atk_md0430_display_on();
    atk_md0430_backlight_on();
#if (ATK_MD0430_USING_TOUCH != 0)
    ret = atk_md0430_touch_init();
    if (ret != ATK_MD0430_TOUCH_EOK)
    {
        return ATK_MD0430_ERROR;
    }
#endif
    
    return ATK_MD0430_EOK;
}

/**
 * @brief       ��ȡATK-MD0430ģ��LCD����
 * @param       ��
 * @retval      ATK-MD0430ģ��LCD����
 */
uint16_t atk_md0430_get_lcd_width(void)
{
    return g_atk_md0430_sta.width;
}

/**
 * @brief       ��ȡATK-MD0430ģ��LCD�߶�
 * @param       ��
 * @retval      ATK-MD0430ģ��LCD�߶�
 */
uint16_t atk_md0430_get_lcd_height(void)
{
    return g_atk_md0430_sta.height;
}

/**
 * @brief       ����ATK-MD0430ģ��LCD����
 * @param       ��
 * @retval      ��
 */
void atk_md0430_backlight_on(void)
{
    ATK_MD0430_BL(1);
}

/**
 * @brief       �ر�ATK-MD0430ģ��LCD����
 * @param       ��
 * @retval      ��
 */
void atk_md0430_backlight_off(void)
{
    ATK_MD0430_BL(0);
}

/**
 * @brief       ����ATK-MD0430ģ��LCD��ʾ
 * @param       ��
 * @retval      ��
 */
void atk_md0430_display_on(void)
{
	if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID1)
    {
        atk_md0430_fsmc_write_cmd(0x2900);
    }
    else if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID2)
    {
        atk_md0430_fsmc_write_cmd(0x29);
    }
}

/**
 * @brief       �ر�ATK-MD0430ģ��LCD��ʾ
 * @param       ��
 * @retval      ��
 */
void atk_md0430_display_off(void)
{
	if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID1)
    {
        atk_md0430_fsmc_write_cmd(0x2800);
    }
    else if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID2)
    {
        atk_md0430_fsmc_write_cmd(0x28);
    }
}

/**
 * @brief       ����ATK-MD0430ģ��LCDɨ�跽��
 * @param       scan_dir: ATK_MD0430_LCD_SCAN_DIR_L2R_U2D: �����ң����ϵ���
 *                        ATK_MD0430_LCD_SCAN_DIR_L2R_D2U: �����ң����µ���
 *                        ATK_MD0430_LCD_SCAN_DIR_R2L_U2D: ���ҵ��󣬴��ϵ���
 *                        ATK_MD0430_LCD_SCAN_DIR_R2L_D2U: ���ҵ��󣬴��µ���
 *                        ATK_MD0430_LCD_SCAN_DIR_U2D_L2R: ���ϵ��£�������
 *                        ATK_MD0430_LCD_SCAN_DIR_U2D_R2L: ���ϵ��£����ҵ���
 *                        ATK_MD0430_LCD_SCAN_DIR_D2U_L2R: ���µ��ϣ�������
 *                        ATK_MD0430_LCD_SCAN_DIR_D2U_R2L: ���µ��ϣ����ҵ���
 * @retval      ATK_MD0430_EOK   : ����ATK-MD0430ģ��LCDɨ�跽��ɹ�
 *              ATK_MD0430_ERROR : ����ATK-MD0430ģ��LCDɨ�跽��ʧ��
*               ATK_MD0430_EINVAL: �����������
 */
uint8_t atk_md0430_set_scan_dir(atk_md0430_lcd_scan_dir_t scan_dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    
    switch (g_atk_md0430_sta.disp_dir)
    {
        case ATK_MD0430_LCD_DISP_DIR_0:
        {
            regval = (uint16_t)scan_dir;
            break;
        }
        case ATK_MD0430_LCD_DISP_DIR_90:
        {
            switch (scan_dir)
            {
                case ATK_MD0430_LCD_SCAN_DIR_L2R_U2D:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_U2D_R2L;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_L2R_D2U:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_U2D_L2R;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_R2L_U2D:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_D2U_R2L;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_R2L_D2U:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_D2U_L2R;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_U2D_L2R:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_R2L_U2D;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_U2D_R2L:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_R2L_D2U;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_D2U_L2R:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_L2R_U2D;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_D2U_R2L:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_L2R_D2U;
                    break;
                }
                default:
                {
                    return ATK_MD0430_EINVAL;
                }
            }
            break;
        }
        case ATK_MD0430_LCD_DISP_DIR_180:
        {
            switch (scan_dir)
            {
                case ATK_MD0430_LCD_SCAN_DIR_L2R_U2D:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_R2L_D2U;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_L2R_D2U:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_R2L_U2D;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_R2L_U2D:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_L2R_D2U;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_R2L_D2U:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_L2R_U2D;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_U2D_L2R:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_D2U_R2L;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_U2D_R2L:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_D2U_L2R;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_D2U_L2R:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_U2D_R2L;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_D2U_R2L:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_U2D_L2R;
                    break;
                }
                default:
                {
                    return ATK_MD0430_EINVAL;
                }
            }
            break;
        }
        case ATK_MD0430_LCD_DISP_DIR_270:
        {
            switch (scan_dir)
            {
                case ATK_MD0430_LCD_SCAN_DIR_L2R_U2D:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_D2U_L2R;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_L2R_D2U:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_D2U_R2L;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_R2L_U2D:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_U2D_L2R;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_R2L_D2U:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_U2D_R2L;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_U2D_L2R:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_L2R_D2U;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_U2D_R2L:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_L2R_U2D;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_D2U_L2R:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_R2L_D2U;
                    break;
                }
                case ATK_MD0430_LCD_SCAN_DIR_D2U_R2L:
                {
                    regval = ATK_MD0430_LCD_SCAN_DIR_R2L_U2D;
                    break;
                }
                default:
                {
                    return ATK_MD0430_EINVAL;
                }
            }
            break;
        }
        default:
        {
            return ATK_MD0430_ERROR;
        }
    }
    
    g_atk_md0430_sta.scan_dir = (atk_md0430_lcd_scan_dir_t)regval;
	
	if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID1)
	{
		dirreg = 0X3600;
	}
    else if (g_atk_md0430_sta.chip_id == ATK_MD0430_CHIP_ID2)
    {
		dirreg = 0X36; 
	}
	
    atk_md0430_fsmc_write_reg(dirreg, regval);
    atk_md0430_set_column_address(0, g_atk_md0430_sta.width - 1);
    atk_md0430_set_page_address(0, g_atk_md0430_sta.height - 1);
    
    return ATK_MD0430_EOK;
}

/**
 * @brief       ����ATK-MD0430ģ��LCD��ʾ����
 * @param       disp_dir: ATK_MD0430_LCD_DISP_DIR_0  : LCD˳ʱ����ת0����ʾ����
 *                        ATK_MD0430_LCD_DISP_DIR_90 : LCD˳ʱ����ת90����ʾ����
 *                        ATK_MD0430_LCD_DISP_DIR_180: LCD˳ʱ����ת180����ʾ����
 *                        ATK_MD0430_LCD_DISP_DIR_270: LCD˳ʱ����ת270����ʾ����
 * @retval      ATK_MD0430_EOK   : ����ATK-MD0430ģ��LCD��ʾ����ɹ�
 *              ATK_MD0430_EINVAL: �����������
 */
uint8_t atk_md0430_set_disp_dir(atk_md0430_lcd_disp_dir_t disp_dir)
{
    switch (disp_dir)
    {
        case ATK_MD0430_LCD_DISP_DIR_0:
        {
            g_atk_md0430_sta.width = ATK_MD0430_LCD_WIDTH;
            g_atk_md0430_sta.height = ATK_MD0430_LCD_HEIGHT;
            break;
        }
        case ATK_MD0430_LCD_DISP_DIR_90:
        {
            g_atk_md0430_sta.width = ATK_MD0430_LCD_HEIGHT;
            g_atk_md0430_sta.height = ATK_MD0430_LCD_WIDTH;
            break;
        }
        case ATK_MD0430_LCD_DISP_DIR_180:
        {
            g_atk_md0430_sta.width = ATK_MD0430_LCD_WIDTH;
            g_atk_md0430_sta.height = ATK_MD0430_LCD_HEIGHT;
            break;
        }
        case ATK_MD0430_LCD_DISP_DIR_270:
        {
            g_atk_md0430_sta.width = ATK_MD0430_LCD_HEIGHT;
            g_atk_md0430_sta.height = ATK_MD0430_LCD_WIDTH;
            break;
        }
        default:
        {
            return ATK_MD0430_EINVAL;
        }
    }
    
    g_atk_md0430_sta.disp_dir = disp_dir;
    atk_md0430_set_scan_dir(ATK_MD0430_LCD_SCAN_DIR_L2R_U2D);
    
    return ATK_MD0430_EOK;
}

/**
 * @brief       ��ȡATK-MD0430ģ��LCDɨ�跽��
 * @param       ��
 * @retval      ATK-MD0430ģ��LCDɨ�跽��
 */
atk_md0430_lcd_scan_dir_t atk_md0430_get_scan_dir(void)
{
    return g_atk_md0430_sta.scan_dir;
}

/**
 * @brief       ��ȡATK-MD0430ģ��LCD��ʾ����
 * @param       ��
 * @retval      ATK-MD0430ģ��LCD��ʾ����
 */
atk_md0430_lcd_disp_dir_t atk_md0430_get_disp_dir(void)
{
    return g_atk_md0430_sta.disp_dir;
}

/**
 * @brief       ATK-MD0430ģ��LCD�������
 * @param       xs   : ������ʼX����
 *              ys   : ������ʼY����
 *              xe   : ������ֹX����
 *              ye   : ������ֹY����
 *              color: ���������ɫ
 * @retval      ��
 */
void atk_md0430_fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
    uint16_t x_index;
    uint16_t y_index;
    
    atk_md0430_set_column_address(xs, xe);
    atk_md0430_set_page_address(ys, ye);
    atk_md0430_start_write_memory();
    for (y_index=ys; y_index<=ye; y_index++)
    {
        for (x_index=xs; x_index<= xe; x_index++)
        {
            atk_md0430_fsmc_write_dat(color);
        }
    }
}

/**
 * @brief       ATK-MD0430ģ��LCD����
 * @param       color: ������ɫ
 * @retval      ��
 */
void atk_md0430_clear(uint16_t color)
{
    atk_md0430_fill(0, 0, ATK_MD0430_LCD_WIDTH - 1, ATK_MD0430_LCD_HEIGHT - 1, color);
}

/**
 * @brief       ATK-MD0430ģ��LCD����
 * @param       x    : �������X����
 *              y    : �������Y����
 *              color: ���������ɫ
 * @retval      ��
 */
void atk_md0430_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    atk_md0430_set_column_address(x, x);
    atk_md0430_set_page_address(y, y);
    atk_md0430_start_write_memory();
    atk_md0430_fsmc_write_dat(color);
}

/**
 * @brief       ATK-MD0430ģ��LCD����
 * @param       x    : �������X����
 *              y    : �������Y����
 * @retval      ���������ɫ
 */
uint16_t atk_md0430_read_point(uint16_t x, uint16_t y)
{
    uint16_t color;
    uint8_t color_r;
    uint8_t color_g;
    uint8_t color_b;
    
    if ((x >= g_atk_md0430_sta.width) || (y >= g_atk_md0430_sta.height))
    {
        return ATK_MD0430_EINVAL;
    }
    
    atk_md0430_set_column_address(x, x);
    atk_md0430_set_page_address(y, y);
    atk_md0430_start_read_memory();
    
    color = atk_md0430_fsmc_read_dat(); /* Dummy */
    color = atk_md0430_fsmc_read_dat(); /* [15:11]: R, [7:2]:G */
    color_r = (uint8_t)(color >> 11) & 0x1F;
    color_g = (uint8_t)(color >> 2) & 0x3F;
    color = atk_md0430_fsmc_read_dat(); /* [15:11]: B */
    color_b = (uint8_t)(color >> 11) & 0x1F;
    
    return (uint16_t)(color_r << 11) | (color_g << 5) | color_b;
}

/**
 * @brief       ATK-MD0430ģ��LCD���߶�
 * @param       x1   : �����߶ζ˵�1��X����
 *              y1   : �����߶ζ˵�1��Y����
 *              x2   : �����߶ζ˵�2��X����
 *              y2   : �����߶ζ˵�2��Y����
 *              color: �����߶ε���ɫ
 * @retval      ��
 */
void atk_md0430_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t x_delta;
    uint16_t y_delta;
    int16_t x_sign;
    int16_t y_sign;
    int16_t error;
    int16_t error2;
    
    x_delta = (x1 < x2) ? (x2 - x1) : (x1 - x2);
    y_delta = (y1 < y2) ? (y2 - y1) : (y1 - y2);
    x_sign = (x1 < x2) ? 1 : -1;
    y_sign = (y1 < y2) ? 1 : -1;
    error = x_delta - y_delta;
    
    atk_md0430_draw_point(x2, y2, color);
    
    while ((x1 != x2) || (y1 != y2))
    {
        atk_md0430_draw_point(x1, y1, color);
        
        error2 = error << 1;
        if (error2 > -y_delta)
        {
            error -= y_delta;
            x1 += x_sign;
        }
        
        if (error2 < x_delta)
        {
            error += x_delta;
            y1 += y_sign;
        }
    }
}

/**
 * @brief       ATK-MD0430ģ��LCD�����ο�
 * @param       x1   : �������ο�˵�1��X����
 *              y1   : �������ο�˵�1��Y����
 *              x2   : �������ο�˵�2��X����
 *              y2   : �������ο�˵�2��Y����
 *              color: �������ο����ɫ
 * @retval      ��
 */
void atk_md0430_draw_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    atk_md0430_draw_line(x1, y1, x2, y1, color);
    atk_md0430_draw_line(x1, y2, x2, y2, color);
    atk_md0430_draw_line(x1, y1, x1, y2, color);
    atk_md0430_draw_line(x2, y1, x2, y2, color);
}

/**
 * @brief       ATK-MD0430ģ��LCD��Բ�ο�
 * @param       x    : ����Բ�ο�ԭ���X����
 *              y    : ����Բ�ο�ԭ���Y����
 *              r    : ����Բ�ο�İ뾶
 *              color: ����Բ�ο����ɫ
 * @retval      ��
 */
void atk_md0430_draw_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    int32_t x_t;
    int32_t y_t;
    int32_t error;
    int32_t error2;
    
    x_t = -r;
    y_t = 0;
    error = 2 - 2 * r;
    
    do {
        atk_md0430_draw_point(x - x_t, y + y_t, color);
        atk_md0430_draw_point(x + x_t, y + y_t, color);
        atk_md0430_draw_point(x + x_t, y - y_t, color);
        atk_md0430_draw_point(x - x_t, y - y_t, color);
        
        error2 = error;
        if (error2 <= y_t)
        {
            y_t++;
            error = error + (y_t * 2 + 1);
            if ((-x_t == y_t) && (error2 <= x_t))
            {
                error2 = 0;
            }
        }
        
        if (error2 > x_t)
        {
            x_t++;
            error = error + (x_t * 2 + 1);
        }
    } while (x_t <= 0);
}

/**
 * @brief       ATK-MD0430ģ��LCD��ʾ1���ַ�
 * @param       x    : ����ʾ�ַ���X����
 *              y    : ����ʾ�ַ���Y����
 *              ch   : ����ʾ�ַ�
 *              font : ����ʾ�ַ�������
 *              color: ����ʾ�ַ�����ɫ
 * @retval      ��
 */
void atk_md0430_show_char(uint16_t x, uint16_t y, char ch, atk_md0430_lcd_font_t font, uint16_t color)
{
    const uint8_t *ch_code;
    uint8_t ch_width;
    uint8_t ch_height;
    uint8_t ch_size;
    uint8_t ch_offset;
    uint8_t byte_index;
    uint8_t byte_code;
    uint8_t bit_index;
    uint8_t width_index = 0;
    uint8_t height_index = 0;
    
    ch_offset = ch - ' ';
    
    switch (font)
    {
#if (ATK_MD0430_FONT_12 != 0)
        case ATK_MD0430_LCD_FONT_12:
        {
            ch_code = atk_md0430_font_1206[ch_offset];
            ch_width = ATK_MD0430_FONT_12_CHAR_WIDTH;
            ch_height = ATK_MD0430_FONT_12_CHAR_HEIGHT;
            ch_size = ATK_MD0430_FONT_12_CHAR_SIZE;
            break;
        }
#endif
#if (ATK_MD0430_FONT_16 != 0)
        case ATK_MD0430_LCD_FONT_16:
        {
            ch_code = atk_md0430_font_1608[ch_offset];
            ch_width = ATK_MD0430_FONT_16_CHAR_WIDTH;
            ch_height = ATK_MD0430_FONT_16_CHAR_HEIGHT;
            ch_size = ATK_MD0430_FONT_16_CHAR_SIZE;
            break;
        }
#endif
#if (ATK_MD0430_FONT_24 != 0)
        case ATK_MD0430_LCD_FONT_24:
        {
            ch_code = atk_md0430_font_2412[ch_offset];
            ch_width = ATK_MD0430_FONT_24_CHAR_WIDTH;
            ch_height = ATK_MD0430_FONT_24_CHAR_HEIGHT;
            ch_size = ATK_MD0430_FONT_24_CHAR_SIZE;
            break;
        }
#endif
#if (ATK_MD0430_FONT_32 != 0)
        case ATK_MD0430_LCD_FONT_32:
        {
            ch_code = atk_md0430_font_3216[ch_offset];
            ch_width = ATK_MD0430_FONT_32_CHAR_WIDTH;
            ch_height = ATK_MD0430_FONT_32_CHAR_HEIGHT;
            ch_size = ATK_MD0430_FONT_32_CHAR_SIZE;
            break;
        }
#endif
        default:
        {
            return;
        }
    }
    
    if ((x + ch_width > ATK_MD0430_LCD_WIDTH) || (y + ch_height > ATK_MD0430_LCD_HEIGHT))
    {
        return;
    }
    
    for (byte_index=0; byte_index<ch_size; byte_index++)
    {
        byte_code = ch_code[byte_index];
        for (bit_index=0; bit_index<8; bit_index++)
        {
            if ((byte_code & 0x80) != 0)
            {
                atk_md0430_draw_point(x + width_index, y + height_index, color);
            }
            height_index++;
            if (height_index == ch_height)
            {
                height_index = 0;
                width_index++;
                break;
            }
            byte_code <<= 1;
        }
    }
}

/**
 * @brief       ATK-MD0430ģ��LCD��ʾ�ַ���
 * @note        ���Զ����кͻ�ҳ
 * @param       x     : ����ʾ�ַ�����X����
 *              y     : ����ʾ�ַ�����Y����
 *              width : ����ʾ�ַ�������ʾ�߶�
 *              height: ����ʾ�ַ�������ʾ����
 *              str   : ����ʾ�ַ���
 *              font  : ����ʾ�ַ���������
 *              color : ����ʾ�ַ�������ɫ
 * @retval      ��
 */
void atk_md0430_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, atk_md0430_lcd_font_t font, uint16_t color)
{
    uint8_t ch_width;
    uint8_t ch_height;
    uint16_t x_raw;
    uint16_t y_raw;
    uint16_t x_limit;
    uint16_t y_limit;
    
    switch (font)
    {
#if (ATK_MD0430_FONT_12 != 0)
        case ATK_MD0430_LCD_FONT_12:
        {
            ch_width = ATK_MD0430_FONT_12_CHAR_WIDTH;
            ch_height = ATK_MD0430_FONT_12_CHAR_HEIGHT;
            break;
        }
#endif
#if (ATK_MD0430_FONT_16 != 0)
        case ATK_MD0430_LCD_FONT_16:
        {
            ch_width = ATK_MD0430_FONT_16_CHAR_WIDTH;
            ch_height = ATK_MD0430_FONT_16_CHAR_HEIGHT;
            break;
        }
#endif
#if (ATK_MD0430_FONT_24 != 0)
        case ATK_MD0430_LCD_FONT_24:
        {
            ch_width = ATK_MD0430_FONT_24_CHAR_WIDTH;
            ch_height = ATK_MD0430_FONT_24_CHAR_HEIGHT;
            break;
        }
#endif
#if (ATK_MD0430_FONT_32 != 0)
        case ATK_MD0430_LCD_FONT_32:
        {
            ch_width = ATK_MD0430_FONT_32_CHAR_WIDTH;
            ch_height = ATK_MD0430_FONT_32_CHAR_HEIGHT;
            break;
        }
#endif
        default:
        {
            return;
        }
    }
    
    x_raw = x;
    y_raw = y;
    x_limit = ((x + width + 1) > ATK_MD0430_LCD_WIDTH) ? ATK_MD0430_LCD_WIDTH : (x + width + 1);
    y_limit = ((y + height + 1) > ATK_MD0430_LCD_HEIGHT) ? ATK_MD0430_LCD_HEIGHT : (y + height + 1);
    
    while ((*str >= ' ') && (*str <= '~'))
    {
        if (x + ch_width >= x_limit)
        {
            x = x_raw;
            y += ch_height;
        }
        
        if (y + ch_height >= y_limit)
        {
            y = x_raw;
            x = y_raw;
        }
        
        atk_md0430_show_char(x, y, *str, font, color);
        
        x += ch_width;
        str++;
    }
}

/**
 * @brief       ATK-MD0430ģ��LCD��ʾ���֣��ɿ�����ʾ��λ0
 * @param       x    : ����ʾ���ֵ�X����
 *              y    : ����ʾ���ֵ�Y����
 *              num  : ����ʾ����
 *              len  : ����ʾ���ֵ�λ��
 *              mode : ATK_MD0430_NUM_SHOW_NOZERO: ���ָ�λ0����ʾ
 *                     ATK_MD0430_NUM_SHOW_ZERO  : ���ָ�λ0��ʾ
 *              font : ����ʾ���ֵ�����
 *              color: ����ʾ���ֵ���ɫ
 * @retval      ��
 */
void atk_md0430_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, atk_md0430_num_mode_t mode, atk_md0430_lcd_font_t font, uint16_t color)
{
    uint8_t ch_width;
    uint8_t len_index;
    uint8_t num_index;
    uint8_t first_nozero = 0;
    char pad;
    
    switch (font)
    {
#if (ATK_MD0430_FONT_12 != 0)
        case ATK_MD0430_LCD_FONT_12:
        {
            ch_width = ATK_MD0430_FONT_12_CHAR_WIDTH;
            break;
        }
#endif
#if (ATK_MD0430_FONT_16 != 0)
        case ATK_MD0430_LCD_FONT_16:
        {
            ch_width = ATK_MD0430_FONT_16_CHAR_WIDTH;
            break;
        }
#endif
#if (ATK_MD0430_FONT_24 != 0)
        case ATK_MD0430_LCD_FONT_24:
        {
            ch_width = ATK_MD0430_FONT_24_CHAR_WIDTH;
            break;
        }
#endif
#if (ATK_MD0430_FONT_32 != 0)
        case ATK_MD0430_LCD_FONT_32:
        {
            ch_width = ATK_MD0430_FONT_32_CHAR_WIDTH;
            break;
        }
#endif
        default:
        {
            return;
        }
    }
    
    switch (mode)
    {
        case ATK_MD0430_NUM_SHOW_NOZERO:
        {
            pad = ' ';
            break;
        }
        case ATK_MD0430_NUM_SHOW_ZERO:
        {
            pad = '0';
            break;
        }
        default:
        {
            return;
        }
    }
    
    for (len_index=0; len_index<len; len_index++)
    {
        num_index = (num / atk_md0430_pow(10, len - len_index - 1)) % 10;
        if ((first_nozero == 0) && (len_index < (len - 1)))
        {
            if (num_index == 0)
            {
                atk_md0430_show_char(x + ch_width * len_index, y, pad, font, color);
                continue;
            }
            else
            {
                first_nozero = 1;
            }
        }
        
        atk_md0430_show_char(x + ch_width * len_index, y, num_index + '0', font, color);
    }
}

/**
 * @brief       ATK-MD0430ģ��LCD��ʾ���֣�����ʾ��λ0
 * @param       x    : ����ʾ���ֵ�X����
 *              y    : ����ʾ���ֵ�Y����
 *              num  : ����ʾ����
 *              len  : ����ʾ���ֵ�λ��
 *              font : ����ʾ���ֵ�����
 *              color: ����ʾ���ֵ���ɫ
 * @retval      ��
 */
void atk_md0430_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, atk_md0430_lcd_font_t font, uint16_t color)
{
    atk_md0430_show_xnum(x, y, num, len, ATK_MD0430_NUM_SHOW_NOZERO, font, color);
}

/**
 * @brief       ATK-MD0430ģ��LCDͼƬ
 * @note        ͼƬȡģ��ʽ: ˮƽɨ�衢RGB565����λ��ǰ
 * @param       x     : ����ʾͼƬ��X����
 *              y     : ����ʾͼƬ��Y����
 *              width : ����ʾͼƬ�Ŀ���
 *              height: ����ʾͼƬ�ĸ߶�
 *              pic   : ����ʾͼƬ�����׵�ַ
 * @retval      ��
 */
void atk_md0430_show_pic(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *pic)
{
    uint16_t x_index;
    uint16_t y_index;
    
    if ((x + width > ATK_MD0430_LCD_WIDTH) || (y + height > ATK_MD0430_LCD_HEIGHT))
    {
        return;
    }
    
    atk_md0430_set_column_address(x, x + width - 1);
    atk_md0430_set_page_address(y, y + height - 1);
    atk_md0430_start_write_memory();
    for (y_index=y; y_index<=(y + height); y_index++)
    {
        for (x_index=x; x_index<=(x + width); x_index++)
        {
            atk_md0430_fsmc_write_dat(*pic);
            pic++;
        }
    }
}
