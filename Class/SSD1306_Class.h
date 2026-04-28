#ifndef __SSD1306_CLASS_H__
#define __SSD1306_CLASS_H__

#include "user_main.h"
#include "I2C_Class.h"
#include "SPI_Class.h"
#include "SSD1306_fonts.h"

#include <stdint.h>

#ifdef SSD1306_X_OFFSET
#define SSD1306_X_OFFSET_LOWER (SSD1306_X_OFFSET & 0x0F)
#define SSD1306_X_OFFSET_UPPER ((SSD1306_X_OFFSET >> 4) & 0x07)
#else
#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0
#endif

#define SSD1306_ADDR        (0x3C << 1)

#define SSD1306_HEIGHT          64
#define SSD1306_WIDTH           128

#define SSD1306_BUFFER_SIZE   SSD1306_WIDTH * SSD1306_HEIGHT / 8

typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

typedef enum {
    SSD1306_OK = 0x00,
    SSD1306_ERR = 0x01  // Generic error.
} SSD1306_Error_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} SSD1306_VERTEX;

typedef struct {
	const uint8_t width;                /**< Font width in pixels */
	const uint8_t height;               /**< Font height in pixels */
	const uint16_t *const data;         /**< Pointer to font data array */
    const uint8_t *const char_width;    /**< Proportional character width in pixels (NULL for monospaced) */
} SSD1306_Font_t;


static const SSD1306_Font_t Font_6x8 = {6, 8, Font6x8, NULL};
static const SSD1306_Font_t Font_7x10 = {7, 10, Font7x10, NULL};
static const SSD1306_Font_t Font_11x18 = {11, 18, Font11x18, NULL};
static const SSD1306_Font_t Font_16x26 = {16, 26, Font16x26, NULL};
static const SSD1306_Font_t Font_16x24 = {16, 24, Font16x24, NULL};
static const SSD1306_Font_t Font_16x15 = {16, 15, Font16x15, char_width};


class SSD1306 : public MCU_I2C, public MCU_SPI
{
	private:
		uint16_t CurrentX;
		uint16_t CurrentY;
		uint8_t Initialized;
		uint8_t DisplayOn;
		
		uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];

		bool is_i2c;

		MCU_GPIO SPI_DC;
		MCU_GPIO SPI_RESET;

		const SSD1306_Font_t* font;
	public:
		// Procedure definitions
		void Init(void);
		
		void Reset(void);
		void Fill(SSD1306_COLOR color);
		void UpdateScreen(void);
		void DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
		char WriteChar(char ch, SSD1306_COLOR color);
		char WriteString(char* str, SSD1306_COLOR color);
		void SetCursor(uint8_t x, uint8_t y);
		void Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
		void DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SSD1306_COLOR color);
		void DrawArcWithRadiusLine(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SSD1306_COLOR color);
		void DrawCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, SSD1306_COLOR color);
		void FillCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r,SSD1306_COLOR par_color);
		void Polyline(const SSD1306_VERTEX *par_vertex, uint16_t par_size, SSD1306_COLOR color);
		void DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
		void FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
		SSD1306_Error_t InvertRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
		void DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, SSD1306_COLOR color);
		void SetContrast(const uint8_t value);
		void SetDisplayOn(const uint8_t on);	//0: OFF. 1: ON.
		uint8_t GetDisplayOn() { return DisplayOn; }
		void WriteCommand(uint8_t byte, uint32_t timeout = HAL_MAX_DELAY);
		void WriteData(uint8_t* buffer, size_t buff_size, uint32_t timeout = HAL_MAX_DELAY);
		SSD1306_Error_t FillBuffer(uint8_t* buf, uint32_t len);

		void SetFont(const SSD1306_Font_t* font) { this->font = font; }

		SSD1306(I2C_HandleTypeDef* hi2c, const SSD1306_Font_t* font = &Font_7x10, uint8_t SLAVE_ADDRESS = SSD1306_ADDR, uint16_t MemAddSize = I2C_MEMADD_SIZE_8BIT);

		SSD1306(SPI_HandleTypeDef* hspi, MCU_GPIO cs, MCU_GPIO dc, MCU_GPIO reset, const SSD1306_Font_t* font = &Font_7x10);
		~SSD1306();
};

#endif
