#include "FSMC_Class.h"

MCU_FSMC::MCU_FSMC(uint32_t bank_addr, uint32_t rs_pin)
{
    uint32_t cmd_offset = (((1U << rs_pin) - 1) << 1);
    uint32_t dat_offset = ((1U << rs_pin) << 1);

    CMD_REG = (volatile uint16_t *)(bank_addr | cmd_offset);
    DAT_REG = (volatile uint16_t *)(bank_addr | dat_offset);
}

MCU_FSMC::~MCU_FSMC()
{
}