#include "AHT20_Class.h"

void AHT20::Init(void){
  uint8_t readBuffer;
  Init_Delay(40);
  receive(&readBuffer, 1);
  if ((readBuffer & 0x08) == 0x00)
  {
      uint8_t sendBuffer[3] = {0xBE, 0x08, 0x00};
      transmit(sendBuffer, sizeof(sendBuffer));
  }
}

void AHT20::Read(float *Temperature, float *Humidity)
{
    uint8_t sendBuffer[3] = {0xAC, 0x33, 0x00};
    uint8_t readBuffer[6] = {0};

    transmit(sendBuffer, sizeof(sendBuffer));
    Delay(75);
    receive(readBuffer, sizeof(readBuffer));

    if ((readBuffer[0] & 0x80) == 0x00)
    {
        uint32_t data = 0;
        data = ((uint32_t)readBuffer[3] >> 4) + ((uint32_t)readBuffer[2] << 4) + ((uint32_t)readBuffer[1] << 12);
        *Humidity = data * 100.0f / (1 << 20);

        data = (((uint32_t)readBuffer[3] & 0x0F) << 16) + ((uint32_t)readBuffer[4] << 8) + (uint32_t)readBuffer[5];
        *Temperature = data * 200.0f / (1 << 20) - 50;
    }
}


AHT20::AHT20(I2C_HandleTypeDef* hi2c, uint8_t SLAVE_ADDRESS, uint16_t MemAddSize)
    : MCU_I2C(hi2c, SLAVE_ADDRESS, MemAddSize)
{

}

AHT20::~AHT20()
{
}
