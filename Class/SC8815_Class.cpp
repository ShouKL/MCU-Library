#include "SC8815_Class.h"

void SC8815::Init(void)
{
    BatteryConfigStruct = { 0 };
    HardwareInitStruct = { 0 };
    InterruptMaskInitStruct = { 0 };
    /****启动 SC8815...****/
    //->设置 PSTOP 为高
    PSTOP.high();
    //->设置 CE 为低
    Init_Delay(220);   //必要的启动延时

    //配置 SC8815 电池参数选项
    BatteryConfigStruct.IRCOMP = SCBAT_IRCOMP_20mR;
    BatteryConfigStruct.VBAT_SEL = SCBAT_VBAT_SEL_Internal;
    BatteryConfigStruct.CSEL = SCBAT_CSEL_3S;
    BatteryConfigStruct.VCELL = SCBAT_VCELL_4v25;
    BatteryConfig();

    //配置 SC8815 硬件参数选项
    HardwareInitStruct.IBAT_RATIO = SCHWI_IBAT_RATIO_6x;
    HardwareInitStruct.IBUS_RATIO = SCHWI_IBUS_RATIO_3x;
    HardwareInitStruct.VBAT_RATIO = SCHWI_VBAT_RATIO_12_5x;
    HardwareInitStruct.VBUS_RATIO = SCHWI_VBUS_RATIO_12_5x;
		
    HardwareInitStruct.VINREG_Ratio = SCHWI_VINREG_RATIO_100x;
    HardwareInitStruct.SW_FREQ = SCHWI_FREQ_450KHz;
    HardwareInitStruct.DeadTime = SCHWI_DT_80ns;
    HardwareInitStruct.ICHAR = SCHWI_ICHAR_IBAT;
    HardwareInitStruct.TRICKLE = SCHWI_TRICKLE_Disable;
    HardwareInitStruct.TERM = SCHWI_TERM_Disable;
    HardwareInitStruct.FB_Mode = SCHWI_FB_External;
    HardwareInitStruct.TRICKLE_SET = SCHWI_TRICKLE_SET_70;
    HardwareInitStruct.OVP = SCHWI_OVP_Enable;
    HardwareInitStruct.DITHER = SCHWI_DITHER_Disable;
    HardwareInitStruct.SLEW_SET = SCHWI_SLEW_1mV_us;
    HardwareInitStruct.ADC_SCAN = SCHWI_ADC_Enable;
    HardwareInitStruct.ILIM_BW = SCHWI_ILIM_BW_1_25KHz;
    HardwareInitStruct.LOOP = SCHWI_LOOP_Normal;
    HardwareInitStruct.ShortFoldBack = SCHWI_SFB_Enable;
    HardwareInitStruct.EOC = SCHWI_EOC_1_25;
    HardwareInitStruct.PFM = SCHWI_PFM_Disable;
    HardwareInit();

    //配置 SC8815 中断屏蔽选项
    InterruptMaskInitStruct.AC_OK = sENABLE;
    InterruptMaskInitStruct.INDET = sENABLE;
    InterruptMaskInitStruct.VBUS_SHORT = sENABLE;
    InterruptMaskInitStruct.OTP = sENABLE;
    InterruptMaskInitStruct.EOC = sENABLE;
    ConfigInterruptMask();

    Init_Delay(50);
    
    /***现在可以设置 PSTOP 引脚为低, 启动 SC8815 功率转换****/
    PSTOP.low();
}

/****************************************
* @note		进行配置之前先启动 SC8815 并延时 5ms 确保器件就绪,还需要 SC8815 PSTOP 引脚为高才能进行此硬件配置
*****************************************/
void SC8815::BatteryConfig(void)
{
    uint8_t tmp;
    tmp = BatteryConfigStruct.IRCOMP;
    tmp |= BatteryConfigStruct.VBAT_SEL;
    tmp |= BatteryConfigStruct.CSEL;
    tmp |= BatteryConfigStruct.VCELL;
    writeByte(&tmp, SCREG_VBAT_SET);
}
/****************************************
* @note		进行配置之前先启动 SC8815 并延时 5ms 确保器件就绪,还需要 SC8815 PSTOP 引脚为高才能进行此硬件配置
*****************************************/
void SC8815::HardwareInit(void)
{
    uint8_t tmp;

    //比例配置
    readByte(&tmp, SCREG_RATIO);
    tmp &= 0xE0;  //读取寄存器中的保留位(这些保留位不能动)
    tmp |= HardwareInitStruct.IBAT_RATIO;           //装填配置参数
    tmp |= HardwareInitStruct.IBUS_RATIO;
    tmp |= HardwareInitStruct.VBAT_RATIO;
    tmp |= HardwareInitStruct.VBUS_RATIO;
    writeByte(&tmp, SCREG_RATIO);    //写回寄存器

    //硬件配置0
    readByte(&tmp, SCREG_CTRL0_SET);
    tmp &= 0x60;
    tmp |= HardwareInitStruct.VINREG_Ratio;
    tmp |= HardwareInitStruct.SW_FREQ;
    tmp |= HardwareInitStruct.DeadTime;
    writeByte(&tmp, SCREG_CTRL0_SET);

    //硬件配置1
    readByte(&tmp, SCREG_CTRL1_SET);
    tmp &= 0x03;
    tmp |= HardwareInitStruct.ICHAR;
    tmp |= HardwareInitStruct.TRICKLE;
    tmp |= HardwareInitStruct.TERM;
    tmp |= HardwareInitStruct.FB_Mode;
    tmp |= HardwareInitStruct.TRICKLE_SET;
    tmp |= HardwareInitStruct.OVP;
    writeByte(&tmp, SCREG_CTRL1_SET);

    //硬件配置2
    readByte(&tmp, SCREG_CTRL2_SET);
    tmp = (tmp & 0xF0) | 0x08;
    tmp |= HardwareInitStruct.DITHER;
    tmp |= HardwareInitStruct.SLEW_SET;
    writeByte(&tmp, SCREG_CTRL2_SET);

    //硬件配置3(没有保留位)
    tmp = HardwareInitStruct.ADC_SCAN;
    tmp |= HardwareInitStruct.ILIM_BW;
    tmp |= HardwareInitStruct.LOOP;
    tmp |= HardwareInitStruct.ShortFoldBack;
    tmp |= HardwareInitStruct.EOC;
    tmp |= HardwareInitStruct.PFM;
    writeByte(&tmp, SCREG_CTRL3_SET);

    //配置完成后必要的延时
    Init_Delay(5);
}

void SC8815::ConfigInterruptMask(void)
{
    uint8_t tmp;
    readByte(&tmp, SCREG_MASK);
    tmp &= 0x91;
    tmp |= InterruptMaskInitStruct.AC_OK ? 0x40 : 0x00;
    tmp |= InterruptMaskInitStruct.INDET ? 0x20 : 0x00;
    tmp |= InterruptMaskInitStruct.VBUS_SHORT ? 0x08 : 0x00;
    tmp |= InterruptMaskInitStruct.OTP ? 0x04 : 0x00;
    tmp |= InterruptMaskInitStruct.EOC ? 0x02 : 0x00;
    writeByte(&tmp, SCREG_MASK);
}

void SC8815::ReadInterrupStatus(void)
{
    uint8_t tmp;
    readByte(&tmp, SCREG_MASK);     //读取状态寄存器
    InterruptMaskInitStruct.AC_OK = (tmp & 0x40) >> 6;   //拆解出每一个位
    InterruptMaskInitStruct.INDET = (tmp & 0x20) >> 5;
    InterruptMaskInitStruct.VBUS_SHORT = (tmp & 0x08) >> 3;
    InterruptMaskInitStruct.OTP = (tmp & 0x04) >> 2;
    InterruptMaskInitStruct.EOC = (tmp & 0x02) >> 1;
}

uint16_t SC8815::Read_VBUS_Voltage(void)
{
    uint8_t tmp1, tmp2;
    double RATIO_Value;
    
    uint8_t RATIO;
    readByte(&RATIO, SCREG_RATIO);
    RATIO_Value = ((RATIO & 0x01) == 1) ? 5 : 12.5; //取得 VBUS 电压的比率
    readByte(&tmp1, SCREG_VBUS_FB_VALUE);           //读取 ADC 值寄存器1
    readByte(&tmp2, SCREG_VBUS_FB_VALUE2);
    tmp2 >>= 6;     //读取 ADC 值寄存器2

    //返回 VBUS 电压值
    BatteryStatusStruct.VBUS_Voltage = (uint16_t)((4 * tmp1 + tmp2 + 1) * RATIO_Value) * 2;
    return BatteryStatusStruct.VBUS_Voltage;
}

uint16_t SC8815::Read_VBUS_Current(void)
{
    uint8_t tmp1, tmp2;
    uint16_t RATIO_Value;

    uint8_t RATIO;
    readByte(&RATIO, SCREG_RATIO);
    RATIO_Value = ((RATIO & 0x0C) == 4) ? 6 : 3;    //取得 IBUS 的比率
    readByte(&tmp1, SCREG_IBUS_VALUE);              //读取 ADC 值寄存器1
    readByte(&tmp2, SCREG_IBUS_VALUE2);
    tmp2 >>= 6;        //读取 ADC 值寄存器2

    //返回 VBUS 电流值
    BatteryStatusStruct.VBUS_Current = ((uint16_t)(50 * RATIO_Value) * (4 * tmp1 + tmp2 + 1)) / (3 * SCHW_VBUS_RSHUNT);
    return BatteryStatusStruct.VBUS_Current;
}

uint16_t SC8815::Read_BATT_Voltage(void)
{
    uint8_t tmp1, tmp2;
    double RATIO_Value;

    uint8_t RATIO;
    readByte(&RATIO, SCREG_RATIO);
    RATIO_Value = ((RATIO & 0x02) == 2) ? 5 : 12.5; //取得电池电压的比率
    readByte(&tmp1, SCREG_VBAT_FB_VALUE);           //读取 ADC 值寄存器1
    readByte(&tmp2, SCREG_VBAT_FB_VALUE2);
    tmp2 >>= 6;     //读取 ADC 值寄存器2

    //返回电池电压值
    BatteryStatusStruct.BATT_Voltage = (uint16_t)((4 * tmp1 + tmp2 + 1) * RATIO_Value) * 2;
    return BatteryStatusStruct.BATT_Voltage;
}

uint16_t SC8815::Read_BATT_Current(void)
{
    uint8_t tmp1, tmp2;
    uint16_t RATIO_Value;

    uint8_t RATIO;
    readByte(&RATIO, SCREG_RATIO);
    RATIO_Value = ((RATIO & 0x10) == 16) ? 12 : 6;  //取得 IBAT 的比率
    readByte(&tmp1, SCREG_IBAT_VALUE);              //读取 ADC 值寄存器1
    readByte(&tmp2, SCREG_IBAT_VALUE2);
    tmp2 >>= 6;        //读取 ADC 值寄存器2

    //返回 IBAT 电流值
    BatteryStatusStruct.BATT_Current = (uint16_t)((50 * RATIO_Value) * (4 * tmp1 + tmp2 + 1)) / (3 * SCHW_BATT_RSHUNT);
    return BatteryStatusStruct.BATT_Current;
}

uint16_t SC8815::Read_ADIN_Voltage(void)
{
    uint8_t tmp1,tmp2;

    readByte(&tmp1, SCREG_ADIN_VALUE);          //读取 ADC 值寄存器1
    readByte(&tmp2, SCREG_ADIN_VALUE2);
    tmp2 >>= 6;    //读取 ADC 值寄存器2

    //返回 ADIN_PIN 电压值
    BatteryStatusStruct.ADIN_Voltage = (uint16_t)(4 * tmp1 + tmp2 + 1) * 2;
    return BatteryStatusStruct.ADIN_Voltage;
}

/****************************************
* @note     不得输入超出最大可设定值的电压值, 如最大输出 1024mV,  输入 1145mV 将导致计算结果溢出错误
*****************************************/
void SC8815::SetOutputVoltage(uint16_t NewVolt)
{
    uint16_t tmp2;
    double RATIO_Value;

    uint8_t SCREG_CTRL1;
    readByte(&SCREG_CTRL1, SCREG_CTRL1_SET);
    //判断 VBUS 电压反馈的模式
    if (SCREG_CTRL1 & 0x10)
    {
        RATIO_Value = (double)SCHW_FB_RUP / SCHW_FB_RDOWM + 1.0;    //计算输出电压比率
        BatteryStatusStruct.OutputVoltage = (NewVolt / RATIO_Value) / 2;                         //计算对应的参考电压

        //得到 VBUSREF 寄存器 2 的值
        for (tmp2 = 0; tmp2 < 3; tmp2++)
        {
            if (((BatteryStatusStruct.OutputVoltage - tmp2 - 1) % 4) == 0)
            {
                break;
            }
        }

        //得到 VBUSREF 寄存器 1 的值
        BatteryStatusStruct.OutputVoltage = (BatteryStatusStruct.OutputVoltage - tmp2 - 1) / 4;

        //写入到 SC8815 VBUSREF_E_SET 寄存器
        writeByte((uint8_t *)&BatteryStatusStruct.OutputVoltage, SCREG_VBUSREF_E_SET);
        tmp2 <<= 6;
        writeByte((uint8_t *)&tmp2, SCREG_VBUSREF_E_SET2);
    }
    else
    {
        uint8_t RATIO;
        readByte(&RATIO, SCREG_RATIO);
        RATIO_Value = ((RATIO & 0x01) == 1) ? 5 : 12.5; //取得 VBUS 电压的比率
        BatteryStatusStruct.OutputVoltage = NewVolt / RATIO_Value / 2;   //计算对应的参考电压

        //得到 VBUSREF 寄存器 2 的值
        for (tmp2 = 0; tmp2 < 3; tmp2++)
        {
            if (((BatteryStatusStruct.OutputVoltage - tmp2 - 1) % 4) == 0)
            {
                break;
            }
        }

        //得到 VBUSREF 寄存器 1 的值
        BatteryStatusStruct.VBUS_Voltage = (BatteryStatusStruct.OutputVoltage - tmp2 - 1) / 4;
        //写入到 SC8815 VBUSREF_I_SET 寄存器
        writeByte((uint8_t *)&BatteryStatusStruct.OutputVoltage, SCREG_VBUSREF_I_SET);
        tmp2 <<= 6;
        writeByte((uint8_t *)&tmp2, SCREG_VBUSREF_I_SET2);
    }
}
/****************************************
* @note     最小的限流值不应低于 300mA, 不得输入超出最大可设定值的电流值
*****************************************/
void SC8815::SetBusCurrentLimit(uint16_t NewILIM)
{
    uint16_t RATIO_Value;

    uint8_t RATIO;
    readByte(&RATIO, SCREG_RATIO);
    RATIO_Value = ((RATIO & 0x0C) == 4) ? 6 : 3;    //取得 IBUS 的比率
    BatteryStatusStruct.BusCurrentLimit = (16 * (NewILIM) * (SCHW_VBUS_RSHUNT)) / (625 * RATIO_Value) - 1;              //计算 LIM 设置值
    writeByte((uint8_t *)&BatteryStatusStruct.BusCurrentLimit, SCREG_IBUS_LIM_SET);                             //写入到 SC8815 寄存器
}

void SC8815::SetBatteryCurrentLimit(uint16_t NewILIM)
{
    uint16_t RATIO_Value;

    uint8_t RATIO;
    readByte(&RATIO, SCREG_RATIO);
    RATIO_Value = ((RATIO & 0x10) == 16) ? 12 : 6; //取得 IBAT 的比率
    BatteryStatusStruct.BatteryCurrentLimit = (16 * (NewILIM) * (SCHW_VBUS_RSHUNT)) / (625 * RATIO_Value) - 1;             //计算 LIM 设置值
    writeByte((uint8_t *)&BatteryStatusStruct.BatteryCurrentLimit, SCREG_IBAT_LIM_SET);                            //写入到 SC8815 寄存器
}

void SC8815::SetVINREGVoltage(uint16_t NewVolt)
{
    uint16_t RATIO_Value;
    uint8_t SCREG_CTRL0;
    readByte(&SCREG_CTRL0, SCREG_CTRL0_SET);
    RATIO_Value = ((SCREG_CTRL0 & 0x10) == 16) ? 40 : 100;    //取得 VINREG 的比率
    BatteryStatusStruct.VINREGVoltage = (uint8_t)(NewVolt / RATIO_Value) - 1;
    writeByte((uint8_t *)&BatteryStatusStruct.VINREGVoltage, SCREG_VINREG_SET);               //写入到 SC8815 寄存器
}

uint16_t SC8815::GetOutputVoltage(void)
{
    uint8_t tmp1, tmp2;
    double RATIO_Value;

    //判断 VBUS 电压反馈的模式
    uint8_t SCREG_CTRL1;
    readByte(&SCREG_CTRL1, SCREG_CTRL1_SET);
    if (SCREG_CTRL1 & 0x10)
    {
        //读取 VBUSREF_E_SET 寄存器
        readByte(&tmp1, SCREG_VBUSREF_E_SET);
        readByte(&tmp2, SCREG_VBUSREF_E_SET2 >> 6);

        //计算输出电压比率
        RATIO_Value = (double)SCHW_FB_RUP / SCHW_FB_RDOWM + 1.0;
    }
    else
    {
        //读取 VBUSREF_E_SET 寄存器
        readByte(&tmp1, SCREG_VBUSREF_E_SET);
        readByte(&tmp2, SCREG_VBUSREF_E_SET2 >> 6);

        //取得VBUS电压的比率
        uint8_t RATIO;
        readByte(&RATIO, SCREG_RATIO);
        RATIO_Value = ((RATIO & 0x01) == 1) ? 5 : 12.5;
    }

    //返回对应的输出电压
    BatteryStatusStruct.OutputVoltage = (uint16_t)((4 * uint16_t(tmp1) + uint16_t(tmp2) + 1) * RATIO_Value) * 2;
    return BatteryStatusStruct.OutputVoltage;
}

uint16_t SC8815::GetBusCurrentLimit(void)
{
    uint8_t tmp;
    uint16_t RATIO_Value;

    uint8_t RATIO;
    readByte(&RATIO, SCREG_RATIO);
    RATIO_Value = ((RATIO & 0x0C) == 4) ? 6 : 3; //取得 IBUS 的比率
    readByte(&tmp, SCREG_IBUS_LIM_SET);                          //取得 IBUS 限流寄存器值

    //返回IBUS限流值
    BatteryStatusStruct.BusCurrentLimit = (uint16_t)((uint32_t)(625 * (RATIO_Value) * (tmp + 1)) / (16 * (SCHW_VBUS_RSHUNT)));
    return BatteryStatusStruct.BusCurrentLimit;
}

uint16_t SC8815::GetBatteryCurrentLimit(void)
{
    uint8_t tmp;
    uint16_t RATIO_Value;

    uint8_t RATIO;
    readByte(&RATIO, SCREG_RATIO);
    RATIO_Value = ((RATIO & 0x10) == 16) ? 12 : 6; //取得 IBAT 的比率
    readByte(&tmp, SCREG_IBAT_LIM_SET);                            //取得 IBAT 限流寄存器值

    //返回IBAT限流值
    BatteryStatusStruct.BatteryCurrentLimit = (uint16_t)((uint32_t)(625 * (RATIO_Value) * (tmp + 1)) / (16 * (SCHW_BATT_RSHUNT)));
    return BatteryStatusStruct.BatteryCurrentLimit;
}

uint16_t SC8815::GetVINREGVoltage(void)
{
    uint8_t tmp;
    uint16_t RATIO_Value;
    uint8_t SCREG_CTRL0;
    readByte(&SCREG_CTRL0, SCREG_CTRL0_SET);
    RATIO_Value = ((SCREG_CTRL0 & 0x10) == 16) ? 40 : 100; //取得 VINREG 的比率
    readByte(&tmp, SCREG_VINREG_SET);                                    //取得 VINREG 寄存器值
    BatteryStatusStruct.VINREGVoltage = tmp * RATIO_Value;
    return BatteryStatusStruct.VINREGVoltage;
}

uint16_t SC8815::GetMaxOutputVoltage(void)
{
    double RATIO_Value;

    uint8_t SCREG_CTRL1;
    readByte(&SCREG_CTRL1, SCREG_CTRL1_SET);
    //判断 VBUS 电压反馈的模式
    if (SCREG_CTRL1 & 0x10)
    {
        //计算输出电压比率
        RATIO_Value = (double)SCHW_FB_RUP / SCHW_FB_RDOWM + 1.0;
    }
    else
    {
        //取得 VBUS 电压的比率
        uint8_t RATIO;
        readByte(&RATIO, SCREG_RATIO);
        RATIO_Value = ((RATIO & 0x01) == 1) ? 5 : 12.5;
    }

    //返回最高输出电压值
    BatteryStatusStruct.MaxOutputVoltage = 2048 * RATIO_Value;
    return BatteryStatusStruct.MaxOutputVoltage;
}

uint16_t SC8815::GetOutputVoltageStep(void)
{
    double RATIO_Value;

    uint8_t SCREG_CTRL1;
    readByte(&SCREG_CTRL1, SCREG_CTRL1_SET);
    //判断 VBUS 电压反馈的模式
    if (SCREG_CTRL1 & 0x10)
    {
        //计算输出电压比率
        RATIO_Value = (double)SCHW_FB_RUP / SCHW_FB_RDOWM + 1.0;
    }
    else
    {
        //取得 VBUS 电压的比率
        uint8_t RATIO;
        readByte(&RATIO, SCREG_RATIO);
        RATIO_Value = ((RATIO & 0x01) == 1) ? 5 : 12.5;
    }

    //返回输出电压步进值
    BatteryStatusStruct.OutputVoltageStep = 2 * RATIO_Value;
    return BatteryStatusStruct.OutputVoltageStep;
}

uint8_t SC8815::GetVINREGRatio(void)
{
    //返回 VINREG 的增益
    uint8_t SCREG_CTRL0;
    readByte(&SCREG_CTRL0, SCREG_CTRL0_SET);
    BatteryStatusStruct.VINREGRatio = (SCREG_CTRL0 & 0x10) ? 40 : 100;
    return BatteryStatusStruct.VINREGRatio;
}

void SC8815::OTG_Enable(void)
{
    uint8_t SCREG_CTRL0;
    readByte(&SCREG_CTRL0, SCREG_CTRL0_SET);
    //设置 EN_OTG 位
    uint8_t OTG = SCREG_CTRL0 | 0x80;
    writeByte(&OTG, SCREG_CTRL0_SET);
}

void SC8815::OTG_Disable(void)
{
    uint8_t SCREG_CTRL0;
    readByte(&SCREG_CTRL0, SCREG_CTRL0_SET);
    //清除 EN_OTG 位
    uint8_t OTG = SCREG_CTRL0 & 0x7F;
    writeByte(&OTG, SCREG_CTRL0_SET);
}

void SC8815::VINREG_SetRatio_40x(void)
{
    uint8_t SCREG_CTRL0;
    readByte(&SCREG_CTRL0, SCREG_CTRL0_SET);
    //设置 VINREG_RATIO 位
    uint8_t VINREG = SCREG_CTRL0 | 0x10;
    writeByte(&VINREG, SCREG_CTRL0_SET);
}

void SC8815::VINREG_SetRatio_100x(void)
{
    uint8_t SCREG_CTRL0;
    readByte(&SCREG_CTRL0, SCREG_CTRL0_SET);
    //清除 VINREG_RATIO 位
    uint8_t VINREG = SCREG_CTRL0 & 0xEF;
    writeByte(&VINREG, SCREG_CTRL0_SET);
}

void SC8815::OVP_Enable(void)
{
    uint8_t SCREG_CTRL1;
    readByte(&SCREG_CTRL1, SCREG_CTRL1_SET);
    //清除 DIS_OVP 位
    uint8_t OVP = SCREG_CTRL1 & 0xFB;
    writeByte(&OVP, SCREG_CTRL1_SET);
}

void SC8815::OVP_Disable(void)
{
    uint8_t SCREG_CTRL1;
    readByte(&SCREG_CTRL1, SCREG_CTRL1_SET);
    //设置 DIS_OVP 位
    uint8_t OVP = SCREG_CTRL1 | 0x04;
    writeByte(&OVP, SCREG_CTRL1_SET);
}

void SC8815::PGATE_Enable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //设置 EN_PGATE 位
    uint8_t PGATE = SCREG_CTRL3 | 0x80;
    writeByte(&PGATE, SCREG_CTRL3_SET);
}

void SC8815::PGATE_Disable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //清除 EN_PGATE 位
    uint8_t PGATE = SCREG_CTRL3 & 0x7F;
    writeByte(&PGATE, SCREG_CTRL3_SET);
}

void SC8815::GPO_Enable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //设置 GPO_CTRL 位
    uint8_t GPO = SCREG_CTRL3 | 0x40;
    writeByte(&GPO, SCREG_CTRL3_SET);
}

void SC8815::GPO_Disable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //清除 GPO_CTRL 位
    uint8_t GPO = SCREG_CTRL3 & 0xBF;
    writeByte(&GPO, SCREG_CTRL3_SET);
}

void SC8815::ADC_Enable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //设置 AD_START 位
    uint8_t ADC = SCREG_CTRL3 | 0x20;
    writeByte(&ADC, SCREG_CTRL3_SET);
}

void SC8815::ADC_Disable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //清除 AD_START 位
    uint8_t ADC = SCREG_CTRL3 & 0xDF;
    writeByte(&ADC, SCREG_CTRL3_SET);
}

void SC8815::SFB_Enable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //清除 DIS_ShortFoldBack 位
    uint8_t SFB = SCREG_CTRL3 & 0xFB;
    writeByte(&SFB, SCREG_CTRL3_SET);
}

void SC8815::SFB_Disable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //设置 DIS_ShortFoldBack 位
    uint8_t SFB = SCREG_CTRL3 | 0x04;
    writeByte(&SFB, SCREG_CTRL3_SET);
}

void SC8815::PFM_Enable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //设置 EN_PFM 位
    uint8_t PFM = SCREG_CTRL3 | 0x01;
    writeByte(&PFM, SCREG_CTRL3_SET);
}

void SC8815::PFM_Disable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //清除 EN_PFM 位
    uint8_t PFM = SCREG_CTRL3 & 0xFE;
    writeByte(&PFM, SCREG_CTRL3_SET);
}

uint8_t SC8815::OTG_IsEnable(void)
{
    uint8_t SCREG_CTRL0;
    readByte(&SCREG_CTRL0, SCREG_CTRL0_SET);
    //返回 OTG 的状态
    return (SCREG_CTRL0 & 0x80) ? 1 : 0;
}

uint8_t SC8815::OVP_IsEnable(void)
{
    uint8_t SCREG_CTRL1;
    readByte(&SCREG_CTRL1, SCREG_CTRL1_SET);
    //返回 OVP 的状态
    return (SCREG_CTRL1 & 0x04) ? 0 : 1;
}

uint8_t SC8815::PGATE_IsEnable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //返回 PGATE 的状态
    return (SCREG_CTRL3 & 0x80) ? 1 : 0;
}

uint8_t SC8815::GPO_IsEnable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //返回 GPO 的状态
    return (SCREG_CTRL3 & 0x40) ? 1 : 0;
}

uint8_t SC8815::ADC_IsEnable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //返回 ADC 的状态
    return (SCREG_CTRL3 & 0x20) ? 1 : 0;
}

uint8_t SC8815::SFB_IsEnable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //返回 短路保护功能 的状态
    return (SCREG_CTRL3 & 0x04) ? 0 : 1;
}

uint8_t SC8815::PFM_IsEnable(void)
{
    uint8_t SCREG_CTRL3;
    readByte(&SCREG_CTRL3, SCREG_CTRL3_SET);
    //返回 PFM 的状态
    return (SCREG_CTRL3 & 0x01) ? 1 : 0;
}

SC8815::SC8815(MCU_GPIO PSTOP, I2C_HandleTypeDef *hi2c, uint8_t SLAVE_ADDRESS, uint16_t MemAddSize)
             : MCU_I2C(hi2c, SLAVE_ADDRESS, MemAddSize), PSTOP(PSTOP)
{
}

SC8815::~SC8815()
{
}
