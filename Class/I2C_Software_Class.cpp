#include "I2C_software_Class.h"

I2C_Software::I2C_Software(MCU_GPIO SCL, MCU_GPIO SDA, uint8_t i2c_addr) 
            : SCL(SCL), SDA(SDA), i2c_addr(i2c_addr)
{
    stop();
}

I2C_Software::~I2C_Software()
{
}

void I2C_Software::start(void)
{
    SDA.high();
    SCL.high();
    Delay_us(2);
    SDA.low();
    Delay_us(2);
    SCL.low();
    Delay_us(2);
}

void I2C_Software::stop(void)
{
    SDA.low();
    Delay_us(2);
    SCL.high();
    Delay_us(2);
    SDA.high();
    Delay_us(2);
}

uint8_t I2C_Software::wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    
    SDA.high();
    Delay_us(2);
    SCL.high();
    Delay_us(2);
    
    while (SDA.read())
    {
        waittime++;
        
        if (waittime > 250)
        {
            stop();
            rack = 1;
            break;
        }
    }
    
    SCL.low();
    Delay_us(2);
    
    return rack;
}

void I2C_Software::ack(void)
{
    SDA.low();
    Delay_us(2);
    SCL.high();
    Delay_us(2);
    SCL.low();
    Delay_us(2);
    SDA.high();
    Delay_us(2);
}

void I2C_Software::nack(void)
{
    SDA.high();
    Delay_us(2);
    SCL.high();
    Delay_us(2);
    SCL.low();
    Delay_us(2);
}

void I2C_Software::send_byte(uint8_t dat)
{
    uint8_t t;
    
    for (t=0; t<8; t++)
    {
        if((dat & 0x80) >> 7)
        {
            SDA.high();
        }
        else
        {
            SDA.low();
        }
        Delay_us(2);
        SCL.high();
        Delay_us(2);
        SCL.low();
        dat <<= 1;
    }
    SDA.high();
}

uint8_t I2C_Software::recv_byte(uint8_t _ack)
{
    uint8_t i;
    uint8_t dat = 0;
    
    for (i = 0; i < 8; i++ )
    {
        dat <<= 1;
        SCL.high();
        Delay_us(2);
        
        if (SDA.read())
        {
            dat++;
        }
        
        SCL.low();
        Delay_us(2);
    }
    
    if (_ack == 0)
    {
        nack();
    }
    else
    {
        ack();
    }

    return dat;
}

uint8_t I2C_Software::write_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t buf_index;
    uint8_t ret;
    
    start();
    send_byte((i2c_addr << 1) | IIC_WRITE);
    wait_ack();
    send_byte((uint8_t)(reg >> 8) & 0xFF);
    wait_ack();
    send_byte((uint8_t)reg & 0xFF);
    wait_ack();
    
    for (buf_index=0; buf_index<len; buf_index++)
    {
        send_byte(buf[buf_index]);
        ret = wait_ack();
        if (ret != 0)
        {
            break;
        }
    }
    
    stop();
    
    if (ret != 0)
    {
        return IIC_ERROR;
    }
    
    return IIC_EOK;
}

void I2C_Software::read_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t buf_index;
    
    start();
    send_byte((i2c_addr << 1) | IIC_WRITE);
    wait_ack();
    send_byte((uint8_t)(reg >> 8) & 0xFF);
    wait_ack();
    send_byte((uint8_t)reg & 0xFF);
    wait_ack();
    start();
    send_byte((i2c_addr << 1) | IIC_READ);
    wait_ack();
    
    for (buf_index=0; buf_index<len - 1; buf_index++)
    {
        buf[buf_index] = recv_byte(1);
    }
    
    buf[buf_index] = recv_byte(0);
    
    stop();
}
