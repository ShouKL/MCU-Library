#include "AS5600_Class.h"

AS5600::AS5600(I2C_HandleTypeDef* hi2c, ADC_HandleTypeDef* hadc, uint8_t address, uint16_t memAddSize)
  : MCU_I2C(hi2c, address, memAddSize), MCU_ADC(hadc)
{
    useADC = (hadc != nullptr);
    adc_index = 0;
    adc_encoder_dir = 0;
    adc_midpoint = 0;
}

AS5600::AS5600(ADC_HandleTypeDef* hadc)
    : MCU_I2C(nullptr, NULL, NULL), MCU_ADC(hadc)
{
    useADC = (hadc != nullptr);
    adc_index = 0;
    adc_encoder_dir = 0;
    adc_midpoint = 0;
}

uint8_t AS5600::detectMagnet()
{
    uint8_t magStatus = 0;
    if(!read(&magStatus, 1, AS5600Register::_stat, 100)) return 0;
    return (magStatus & 0x20) ? 1 : 0;
}

int16_t AS5600::getRawAngle()
{
    if(useADC) {
        if(Start() != HAL_OK) return -1;
        uint32_t v = MCU_ADC::GetValue();
        Stop();
        uint16_t raw = (uint16_t)(v & 0x0FFFu);
        uint16_t processed = (uint16_t)(((uint32_t)raw + 4096u - ((adc_midpoint + 3072u) & 0x0FFFu)) & 0x0FFFu);
        return (int16_t)processed;
    }

    uint8_t buf[2] = {0};
    if(!read(buf, 2, AS5600Register::_raw_ang_hi, 100)) return -1;
    int16_t val = ((int16_t)buf[0] << 8) | buf[1];
    return val;
}

int16_t AS5600::getMaxAngle()
{
    uint8_t buf[2] = {0};
    if(!read(buf, 2, AS5600Register::_mang_hi, 100)) return -1;
    return ((int16_t)buf[0] << 8) | buf[1];
}

int16_t AS5600::getStartPosition()
{
    uint8_t buf[2] = {0};
    if(!read(buf, 2, AS5600Register::_zpos_hi, 100)) return -1;
    return ((int16_t)buf[0] << 8) | buf[1];
}

int16_t AS5600::getEndPosition()
{
    uint8_t buf[2] = {0};
    if(!read(buf, 2, AS5600Register::_mpos_hi, 100)) return -1;
    return ((int16_t)buf[0] << 8) | buf[1];
}

int16_t AS5600::getScaledAngle()
{
    uint8_t buf[2] = {0};
    if(!read(buf, 2, AS5600Register::_ang_hi, 100)) return -1;
    return ((int16_t)buf[0] << 8) | buf[1];
}

int16_t AS5600::getAgc()
{
    uint8_t v = 0;
    if(!read(&v, 1, AS5600Register::_agc, 100)) return -1;
    return v;
}

int16_t AS5600::getMagnitude()
{
    uint8_t buf[2] = {0};
    if(!read(buf, 2, AS5600Register::_mag_hi, 100)) return -1;
    return ((int16_t)buf[0] << 8) | buf[1];
}

int16_t AS5600::getBurnCount()
{
    uint8_t v = 0;
    if(!read(&v, 1, AS5600Register::_zmco, 100)) return -1;
    return v;
}

uint8_t AS5600::getMagnetStrength()
{
    uint8_t magStatus = 0;
    if(!read(&magStatus, 1, AS5600Register::_stat, 100)) return 0;
    if((magStatus & 0x20) == 0) return 0;
    if(magStatus & 0x10) return 1;
    if(magStatus & 0x08) return 3;
    return 2;
}

int16_t AS5600::setEndPosition(int16_t endAngle)
{
    if(endAngle == -1)
        rawEndAngle = getRawAngle();
    else
        rawEndAngle = endAngle;
    uint8_t buf[2];
    buf[0] = (uint8_t)(rawEndAngle >> 8);
    buf[1] = (uint8_t)(rawEndAngle & 0xFF);
    if(!write(buf, 2, AS5600Register::_mpos_hi, 100)) return -1;
    mPosition = getEndPosition();
    return mPosition;
}

int16_t AS5600::setStartPosition(int16_t startAngle)
{
    if(startAngle == -1)
        rawStartAngle = getRawAngle();
    else
        rawStartAngle = startAngle;
    uint8_t buf[2];
    buf[0] = (uint8_t)(rawStartAngle >> 8);
    buf[1] = (uint8_t)(rawStartAngle & 0xFF);
    if(!write(buf, 2, AS5600Register::_zpos_hi, 100)) return -1;
    zPosition = getStartPosition();
    return zPosition;
}

int16_t AS5600::setMaxAngle(int16_t newMaxAngle)
{
    if(newMaxAngle == -1)
        maxAngle = getRawAngle();
    else
        maxAngle = newMaxAngle;
    uint8_t buf[2];
    buf[0] = (uint8_t)(maxAngle >> 8);
    buf[1] = (uint8_t)(maxAngle & 0xFF);
    if(!write(buf, 2, AS5600Register::_mang_hi, 100)) return -1;
    return getMaxAngle();
}

int8_t AS5600::burnAngle()
{
    int8_t retVal = 1;
    zPosition = getStartPosition();
    mPosition = getEndPosition();
    maxAngle  = getMaxAngle();

    if(detectMagnet() == 1)
    {
        if(getBurnCount() < 3)
        {
            if((zPosition == 0)&&(mPosition ==0))
                retVal = -3;
            else {
                uint8_t v = 0x80;
                if(!write(&v,1,AS5600Register::_burn,100)) retVal = -1;
            }
        }
        else
            retVal = -2;
    }
    else
        retVal = -1;

    return retVal;
}

int8_t AS5600::burnMaxAngleAndConfig()
{
    int8_t retVal = 1;
    maxAngle  = getMaxAngle();

    if(getBurnCount() == 0)
    {
        if(maxAngle * 0.087f < 18.0f)
            retVal = -2;
        else {
            uint8_t v = 0x40;
            if(!write(&v,1,AS5600Register::_burn,100)) retVal = -1;
        }
    }
    else
        retVal = -1;

    return retVal;
}

float AS5600::convertRawAngleToDegrees(int16_t newAngle)
{
    float retVal = newAngle * 0.087f;
    return retVal;
}

#endif
