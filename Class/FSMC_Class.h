#ifndef __FSMC_CLASS_H__
#define __FSMC_CLASS_H__

#include "user_main.h"

// 抛弃hal库，采用寄存器直接操作方式，提升性能
class MCU_FSMC
{
protected:
    volatile uint16_t* CMD_REG;
    volatile uint16_t* DAT_REG;

public:
    MCU_FSMC(uint32_t bank_addr, uint32_t rs_pin);
    ~MCU_FSMC();

    // 基础读写接口
    inline void write_cmd(volatile uint16_t cmd)   { *CMD_REG = cmd; }
    inline void write_dat(volatile uint16_t dat)   { *DAT_REG = dat; }
    inline void write_reg(volatile uint16_t reg, volatile uint16_t dat)       { *CMD_REG = reg; *DAT_REG = dat; }
    inline uint16_t read_dat()            {  __asm("nop"); __asm("nop"); return *DAT_REG; }
};

#endif /* __FSMC_CLASS_H__ */