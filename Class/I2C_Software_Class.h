#ifndef __I2C_SOFTWARE_CLASS_H__
#define __I2C_SOFTWARE_CLASS_H__

#include "user_main.h"
#include "GPIO_Class.h"

#define IIC_WRITE  0
#define IIC_READ   1

#define IIC_EOK    0
#define IIC_ERROR  1

class I2C_Software
{
private:
    uint8_t i2c_addr;
    MCU_GPIO SCL;
    MCU_GPIO SDA;

private:
    void start(void);
    void stop(void);
    uint8_t wait_ack(void);
    void ack(void);
    void nack(void);
    void send_byte(uint8_t dat);
    uint8_t recv_byte(uint8_t ack);
public:
    I2C_Software(MCU_GPIO SCL, MCU_GPIO SDA, uint8_t i2c_addr);
    ~I2C_Software();
    uint8_t write_reg(uint16_t reg, uint8_t *buf, uint8_t len);
    void read_reg(uint16_t reg, uint8_t *buf, uint8_t len);
};

#endif
