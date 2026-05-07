/**
 * @file lv_port_disp_template.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "ATK_MD0430_Class.h"
#include "dma.h"
extern ATK_MD0430 lcd;
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream6;
static volatile bool dma_busy = false;

/* 分段 DMA 传输的块大小（像素数），小于 65535 */
#define DMA_CHUNK_SIZE  60000
static uint16_t *dma_src_ptr;          // 当前段源地址
static uint32_t dma_remain;            // 剩余未传输像素数
static lv_display_t *current_disp = NULL;
/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES    800
#define MY_DISP_VER_RES    480

#ifndef MY_DISP_HOR_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
    #define MY_DISP_HOR_RES    300
#endif

#ifndef MY_DISP_VER_RES
    #warning Please define or replace the macro MY_DISP_VER_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    240
#endif

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void LVGL_LCD_FSMC_DMA_pCallback(DMA_HandleTypeDef *_hdma) {
    
    // 计算实际传输的像素数（本次 chunk 大小）
    uint32_t chunk = dma_remain > DMA_CHUNK_SIZE ? DMA_CHUNK_SIZE : dma_remain;
    dma_src_ptr += chunk;
    dma_remain -= chunk;

    if (dma_remain > 0) {
        /* 还有剩余数据，继续启动下一段 DMA */
        uint32_t next_chunk = dma_remain > DMA_CHUNK_SIZE ? DMA_CHUNK_SIZE : dma_remain;
        HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream6,
                         (uint32_t)dma_src_ptr,
                         (uint32_t)(0x6C000000 | (1 << 7)),  // LCD 数据端口
                         next_chunk);
        // dma_busy 保持 true
    } else {
        /* 全部传输完成 */
        dma_busy = false;
        if (current_disp) {
            lv_display_flush_ready(current_disp);
            current_disp = NULL;
        }
    }
}

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);
    HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream6, HAL_DMA_XFER_CPLT_CB_ID, LVGL_LCD_FSMC_DMA_pCallback);

    #define FULL_BUF_SIZE  (MY_DISP_HOR_RES * MY_DISP_VER_RES)
    static lv_color_t *full_buf = (lv_color_t *)0x68000000;

    lv_display_set_buffers(disp, full_buf, NULL,
                           FULL_BUF_SIZE * sizeof(lv_color_t),
                           LV_DISPLAY_RENDER_MODE_FULL);
    // #define BUF_ROWS 50  // 每次刷新30行，具体数值可以根据实际情况调整
    // #define BUF_SIZE (MY_DISP_HOR_RES * BUF_ROWS)
    // static lv_color_t *buf1 = (lv_color_t *)0x68000000;
    // static lv_color_t *buf2 = (lv_color_t *)(0x68000000 + BUF_SIZE * sizeof(lv_color_t));
    // lv_display_set_buffers(disp, buf1, buf2,
    //                        BUF_SIZE * sizeof(lv_color_t),
    //                        LV_DISPLAY_RENDER_MODE_PARTIAL);

#if 0
    /* Example 1
     * One buffer for partial rendering*/
    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_1_1[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];            /*A buffer for 10 rows*/
    lv_display_set_buffers(disp, buf_1_1, NULL, sizeof(buf_1_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* Example 2
     * Two buffers for partial rendering
     * In flush_cb DMA or similar hardware should be used to update the display in the background.*/
    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_2_1[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];

    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_2_2[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];
    lv_display_set_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* Example 3
     * Two buffers screen sized buffer for double buffering.
     * Both LV_DISPLAY_RENDER_MODE_DIRECT and LV_DISPLAY_RENDER_MODE_FULL works, see their comments*/
    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];

    LV_ATTRIBUTE_MEM_ALIGN
    static uint8_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];
    lv_display_set_buffers(disp, buf_3_1, buf_3_2, sizeof(buf_3_1), LV_DISPLAY_RENDER_MODE_DIRECT);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    lcd.init();
    /*You code here*/
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
/* 如果上一次传输还未完成，直接返回（LVGL 不会并发调用） */
    if (dma_busy) return;

    uint16_t *color_p = (uint16_t *)px_map;

    lcd.set_column_address(0, MY_DISP_HOR_RES - 1);
    lcd.set_page_address(0, MY_DISP_VER_RES - 1);
    lcd.start_write_memory();

    dma_src_ptr  = color_p;
    dma_remain   = (uint32_t)MY_DISP_HOR_RES * MY_DISP_VER_RES;
    current_disp = disp_drv;

    /* 启动第一段 DMA */
    uint32_t chunk = dma_remain > DMA_CHUNK_SIZE ? DMA_CHUNK_SIZE : dma_remain;
    dma_busy = true;
    HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream6,
                     (uint32_t)dma_src_ptr,
                     (uint32_t)(0x6C000000 | (1 << 7)),
                     chunk);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
