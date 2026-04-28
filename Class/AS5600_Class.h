#ifndef __AS5600_CLASS_H__
#define __AS5600_CLASS_H__

#include "user_main.h"
#include "I2C_Class.h"
#include "ADC_Class.h"

#define AS5600_ADDRESS           0x36 << 1


#define MAX_VALUE 4095

#define PROCESS_VALUE(raw, zero) \
    (((raw) + 4096u - (((zero) + 3072u) & 0x0FFFu)) & 0x0FFFu)

extern uint16_t AD_Value[4];

void StarAndGetResult(void);



typedef enum
{
  _zmco = 0x00,
  _zpos_hi = 0x01,
  _zpos_lo = 0x02,
  _mpos_hi = 0x03,
  _mpos_lo = 0x04,
  _mang_hi = 0x05,
  _mang_lo = 0x06,
  _conf_hi = 0x07,    
  _conf_lo = 0x08,
  _raw_ang_hi = 0x0c,
  _raw_ang_lo = 0x0d,
  _ang_hi = 0x0e,
  _ang_lo = 0x0f,
  _stat = 0x0b,
  _agc = 0x1a,
  _mag_hi = 0x1b,
  _mag_lo = 0x1c,
  _burn = 0xff
}AS5600Register;

class AS5600 : public MCU_I2C, public MCU_ADC
{
private:
    uint16_t rawStartAngle;
    uint16_t zPosition;
    uint16_t rawEndAngle;
    uint16_t mPosition;
    uint16_t maxAngle; 

    
    bool useADC;
    uint8_t adc_index;
    uint8_t adc_encoder_dir;
    uint16_t adc_midpoint;
public:
    AS5600(I2C_HandleTypeDef* hi2c, ADC_HandleTypeDef* hadc = nullptr, uint8_t address = AS5600_ADDRESS, uint16_t memAddSize = I2C_MEMADD_SIZE_8BIT);

    AS5600(ADC_HandleTypeDef* hadc);

    uint8_t detectMagnet();
    int16_t getRawAngle();
    int16_t getMaxAngle();
    int16_t getStartPosition();
    int16_t getEndPosition();
    int16_t getScaledAngle();
    int16_t getAgc();
    int16_t getMagnitude();
    int16_t getBurnCount();
    uint8_t getMagnetStrength();

    int16_t setEndPosition(int16_t endAngle);
    int16_t setStartPosition(int16_t startAngle);
    int16_t setMaxAngle(int16_t newMaxAngle);

    int8_t burnAngle();
    int8_t burnMaxAngleAndConfig();
    float convertRawAngleToDegrees(int16_t newAngle);
};

#endif 
		

//     uint8_t dect = 0;
//     int16_t rawdata = 0;
//     float degress = 0;

//     dect = detectMagnet();
//     printf("detectMagnet is %d\r\n", dect);
//     rawdata = getRawAngle();
//     printf("rawdata is %d\r\n", rawdata);
//     degress = convertRawAngleToDegrees(rawdata);
//     printf("degress is %f\r\n", degress);