#ifndef __AHT20_CLASS_H__
#define __AHT20_CLASS_H__

#include "user_main.h"
#include "I2C_Class.h"

#define AHT20_ADDRESS 0x70

class AHT20 : public MCU_I2C
{
private:
    uint8_t readBuffer[6] = {0};

public:
    void Init(void);

    AHT20(I2C_HandleTypeDef* hi2c, uint8_t SLAVE_ADDRESS = AHT20_ADDRESS, uint16_t MemAddSize = NULL);
    ~AHT20();

    void Read(float *Temperature, float *Humidity);

};

#endif
