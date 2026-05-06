#ifndef __SC8815_CLASS_H__
#define __SC8815_CLASS_H__

#include "user_main.h"
#include "GPIO_Class.h"
#include "I2C_Class.h"

//设置 SC8815 功率路径上的感测电阻值, 应为 10 或 5 这样的整数 (单位:mOhm)
#define SCHW_VBUS_RSHUNT        10          //VBUS 电流路径上的感测电阻值
#define SCHW_BATT_RSHUNT        10          //电池电流路径上的感测电阻值

//设置 FB 引脚上的分压电阻器值, 使用的 VBUS 反馈模式为外部反馈时, 库通过这些值计算对应输出电压 (单位:Ohm)
#define SCHW_FB_RUP             180000      //RUP 为从 FB 连接到 VBUS 之间的电阻值
#define SCHW_FB_RDOWM           10000       //RDOWM 为从 FB 连接到 GND 之间的电阻值

//功能状态定义
#define sENABLE                 0x01        //使能功能
#define sDISABLE                0x00        //失能功能

//SC8815 寄存器地址定义
#define SC8815_ADDR             0xE8        //SC8815 I2C 设备地址
#define SCREG_VBAT_SET          0x00        //电池配置寄存器
#define SCREG_VBUSREF_I_SET     0x01        //VBUS 内部基准寄存器1
#define SCREG_VBUSREF_I_SET2    0x02        //VBUS 内部基准寄存器2
#define SCREG_VBUSREF_E_SET     0x03        //VBUS 外部基准寄存器1
#define SCREG_VBUSREF_E_SET2    0x04        //VBUS 外部基准寄存器2
#define SCREG_IBUS_LIM_SET      0x05        //VBUS 限流设置寄存器
#define SCREG_IBAT_LIM_SET      0x06        //电池限流设置寄存器
#define SCREG_VINREG_SET        0x07        //VINREG 设置寄存器
#define SCREG_RATIO             0x08        //比率设置寄存器
#define SCREG_CTRL0_SET         0x09        //硬件配置寄存器0
#define SCREG_CTRL1_SET         0x0A        //硬件配置寄存器1
#define SCREG_CTRL2_SET         0x0B        //硬件配置寄存器2
#define SCREG_CTRL3_SET         0x0C        //硬件配置寄存器3
#define SCREG_VBUS_FB_VALUE     0x0D        //VBUS 电压反馈寄存器1
#define SCREG_VBUS_FB_VALUE2    0x0E        //VBUS 电压反馈寄存器2
#define SCREG_VBAT_FB_VALUE     0x0F        //VBAT 电压反馈寄存器1
#define SCREG_VBAT_FB_VALUE2    0x10        //VBAT 电压反馈寄存器2
#define SCREG_IBUS_VALUE        0x11        //BUS 电流反馈寄存器1
#define SCREG_IBUS_VALUE2       0x12        //BUS 电流反馈寄存器2
#define SCREG_IBAT_VALUE        0x13        //BAT 电流反馈寄存器1
#define SCREG_IBAT_VALUE2       0x14        //BAT 电流反馈寄存器2
#define SCREG_ADIN_VALUE        0x15        //ADIN 引脚电压反馈寄存器1
#define SCREG_ADIN_VALUE2       0x16        //ADIN 引脚电压反馈寄存器2
#define SCREG_STATUS            0x17        //中断状态寄存器
#define SCREG_MASK              0x19        //中断屏蔽寄存器

//SC8815 电池配置结构体参数定义
#define SCBAT_IRCOMP_0mR            0x00    //电池内阻为 0 mOhm
#define SCBAT_IRCOMP_20mR           0x40    //电池内阻为 20 mOhm
#define SCBAT_IRCOMP_40mR           0x80    //电池内阻为 40 mOhm
#define SCBAT_IRCOMP_80mR           0xC0    //电池内阻为 80 mOhm
#define SCBAT_VBAT_SEL_Internal     0x00    //电池电压反馈为内部设置(VBATS引脚直接连接电池)
#define SCBAT_VBAT_SEL_External     0x20    //外部设置(电池电压经过分压电阻连接VBATS引脚)
#define SCBAT_CSEL_1S               0x00    //电池节数为1节
#define SCBAT_CSEL_2S               0x08    //电池节数为2节
#define SCBAT_CSEL_3S               0x10    //电池节数为3节
#define SCBAT_CSEL_4S               0x18    //电池节数为4节
#define SCBAT_VCELL_4v10            0x00    //单节电池电压为4.10V
#define SCBAT_VCELL_4v20            0x01    //单节电池电压为4.20V
#define SCBAT_VCELL_4v25            0x02    //单节电池电压为4.25V
#define SCBAT_VCELL_4v30            0x03    //单节电池电压为4.30V
#define SCBAT_VCELL_4v35            0x04    //单节电池电压为4.35V
#define SCBAT_VCELL_4v40            0x05    //单节电池电压为4.40V
#define SCBAT_VCELL_4v45            0x06    //单节电池电压为4.45V
#define SCBAT_VCELL_4v50            0x07    //单节电池电压为4.50V

//SC8815 硬件初始化结构体参数定义
#define SCHWI_IBAT_RATIO_6x         0x00    //电池电流比率为 6x (10mOhm 感测电阻时最大限流为 6A)
#define SCHWI_IBAT_RATIO_12x        0x10    //电池电流比率为 12x (10mOhm 感测电阻时最大限流为 12A)
#define SCHWI_IBUS_RATIO_6x         0x04    //VBUS 电流比率为 6x (10mOhm 感测电阻时最大限流为 6A)
#define SCHWI_IBUS_RATIO_3x         0x08    //VBUS 电流比率为 3x (10mOhm 感测电阻时最大限流为 3A)
#define SCHWI_VBAT_RATIO_12_5x      0x00    //电池电压比率为 12.5x, 只影响 SC8815 内置 ADC 测量范围, 最大测量值为 25.6V (适用于2节以上电池应用)
#define SCHWI_VBAT_RATIO_5x         0x02    //电池电压比率为 5x, 只影响 SC8815 内置 ADC 测量范围, 最大测量值为 10.24V (适用于3节以下电池应用)
#define SCHWI_VBUS_RATIO_12_5x      0x00    //VBUS 电压比率为 12.5x, 影响内置 ADC 测量范围和 OTG 反向输出电压范围, 最大操作值为 25.6V, 步进25mV
#define SCHWI_VBUS_RATIO_5x         0x01    //VBUS 电压比率为 5x, 影响内置 ADC 测量范围和 OTG 反向输出电压范围, 最大操作值为 10.24V,  步进10mV
#define SCHWI_VINREG_RATIO_100x     0x00    //VINREG 比率为 100x (100mV 步进调整,最大值为 25.6V)
#define SCHWI_VINREG_RATIO_40x      0x10    //VINREG 比率为 40x (40mV 步进调整,最大值为 10.24V)
#define SCHWI_FREQ_150KHz           0x00    //开关频率为150KHz (升压和降压模式都是 150KHz)
#define SCHWI_FREQ_300KHz_1         0x04    //开关频率为300KHz (升压模式 300KHz, 降压模式 150KHz)
#define SCHWI_FREQ_300KHz_2         0x08    //开关频率为300KHz (升压和降压模式都是 300KHz)
#define SCHWI_FREQ_450KHz           0x0C    //开关频率为450KHz (升压和降压模式都是 450KHz)
#define SCHWI_DT_20ns               0x00    //死区时间为20ns
#define SCHWI_DT_40ns               0x01    //死区时间为40ns
#define SCHWI_DT_60ns               0x02    //死区时间为60ns
#define SCHWI_DT_80ns               0x03    //死区时间为80ns
#define SCHWI_ICHAR_IBUS            0x00    //选择 VBUS 电流作为参考
#define SCHWI_ICHAR_IBAT            0x80    //选择 电池(IBAT) 电流作为参考
#define SCHWI_TRICKLE_Enable        0x00    //打开涓流充电
#define SCHWI_TRICKLE_Disable       0x40    //关闭涓流充电
#define SCHWI_TERM_Enable           0x00    //打开充电自动终止
#define SCHWI_TERM_Disable          0x20    //关闭充电自动终止
#define SCHWI_FB_Internal           0x00    //OTG 反向放电模式 VBUS 电压反馈使用 VBUS_PIN 在芯片内部反馈
#define SCHWI_FB_External           0x10    //OTG 反向放电模式 VBUS 电压反馈使用 FB_PIN 上的外部分压电阻网络反馈
#define SCHWI_TRICKLE_SET_70        0x00    //涓流充电相位闸值为电池电压设置的70%
#define SCHWI_TRICKLE_SET_60        0x08    //涓流充电相位闸值为电池电压设置的60%
#define SCHWI_OVP_Enable            0x00    //打开 SC8815 反向放电模式下的 VBUS 过压保护
#define SCHWI_OVP_Disable           0x04    //关闭 SC8815 反向放电模式下的 VBUS 过压保护
#define SCHWI_DITHER_Disable        0x00    //关闭 SC8815 频率抖动功能,PGATE引脚用于作PMOS控制
#define SCHWI_DITHER_Enable         0x04    //打开 SC8815 频率抖动功能,PGATE引脚用于设置频率抖动
#define SCHWI_SLEW_1mV_us           0x00    //反向放电模式下 VBUS 动态变化的速率 = 1mV/us
#define SCHWI_SLEW_2mV_us           0x01    //反向放电模式下 VBUS 动态变化的速率 = 2mV/us
#define SCHWI_SLEW_4mV_us           0x02    //反向放电模式下 VBUS 动态变化的速率 = 4mV/us
#define SCHWI_SLEW_8mV_us           0x03    //反向放电模式下 VBUS 动态变化的速率 = 8mV/us
#define SCHWI_ADC_Disable           0x00    //关闭 SC8815 ADC 转换,节省1-2mA的功耗
#define SCHWI_ADC_Enable            0x20    //打开 SC8815 ADC 转换,此时可以读取电压电流数据
#define SCHWI_ILIM_BW_5KHz          0x00    //SC8815 电流限制环路的带宽为 5KHz
#define SCHWI_ILIM_BW_1_25KHz       0x10    //SC8815 电流限制环路的带宽为 1.25KHz
#define SCHWI_LOOP_Normal           0x00    //SC8815 环路响应模式为 正常环路响应
#define SCHWI_LOOP_Improved         0x08    //SC8815 环路响应模式为 改善环路响应
#define SCHWI_SFB_Enable            0x00    //打开 SC8815 反向放电模式中的 VBUS 和 BATT 短路保护
#define SCHWI_SFB_Disable           0x04    //关闭 SC8815 反向放电模式中的 VBUS 和 BATT 短路保护
#define SCHWI_EOC_1_25              0x00    //SC8815 充电结束的检测闸值为充电电流设置的1/25
#define SCHWI_EOC_1_10              0x02    //SC8815 充电结束的检测闸值为充电电流设置的1/10
#define SCHWI_PFM_Disable           0x00    //关闭 SC8815 反向放电中轻载下的 PFM 模式
#define SCHWI_PFM_Enable            0x01    //打开 SC8815 反向放电中轻载下的 PFM 模式

//SC8815 电池配置结构体定义
typedef struct
{
    uint8_t IRCOMP;             /*电池内阻补偿设置-这个参数应取 SCBAT_IRCOMP 中定义的值*/
    uint8_t VBAT_SEL;           /*电池电压反馈模式设置-这个参数应取 SCBAT_VSEL 中定义的值*/
    uint8_t CSEL;               /*电池串联节数设置-这个参数应取 SCBAT_CSEL 中定义的值*/
    uint8_t VCELL;              /*设置单节电池的电压-这个参数应取 SCBAT_VCELL 中定义的值*/
}BatteryConfig;

//SC8815 硬件初始化结构体定义
typedef struct
{
    uint8_t IBAT_RATIO;         /*电池电流比率设置              -这个参数应取 SCHWI_IBAT_RATIO 中定义的值*/
    uint8_t IBUS_RATIO;         /*VBUS 电流比率设置             -这个参数应取 SCHWI_IBUS_RATIO 中定义的值*/
    uint8_t VBAT_RATIO;         /*电池电压比率设置              -这个参数应取 SCHWI_VBAT_RATIO 中定义的值*/
    uint8_t VBUS_RATIO;         /*VBUS 电压比率设置             -这个参数应取 SCHWI_VBUS_RATIO 中定义的值*/
    uint8_t VINREG_Ratio;       /*VINREG 电压比率设置           -这个参数应取 SCHWI_VINREG_RATIO 中定义的值*/
    uint8_t SW_FREQ;            /*设置 SC8815 的开关频率        -这个参数应取 SCHWI_FREQ 中定义的值*/
    uint8_t DeadTime;           /*设置开关死区时间              -这个参数应取 SCHWI_DT 中定义的值*/
    uint8_t ICHAR;              /*设置充电电流参考,涓流充电电流和终止电流将基于此参考-这个参数应取 SCHWI_ICHAR 中定义的值*/
    uint8_t TRICKLE;            /*设置涓流充电功能的关闭和打开      -这个参数应取 SCHWI_TRICKLE 中定义的值*/
    uint8_t TERM;               /*打开或关闭充电自动终止功能        -这个参数应取 SCHWI_TERM 中定义的值*/
    uint8_t FB_Mode;            /*设置 SC8815 反向放电模式的 VBUS 电压反馈模式-这个参数应取 SCHWI_FB_MODE 中定义的值*/
    uint8_t TRICKLE_SET;        /*设置涓流充电相位闸值              -这个参数应取 SCHWI_TRICKLE_SET 中定义的值*/
    uint8_t OVP;                /*打开或关闭 SC8815 反向放电模式中的过压保护功能-这个参数应取 SCHWI_OVP 中定义的值*/
    uint8_t DITHER;             /*打开或关闭 SC8815 频率抖动功能    -这个参数应取 SCHWI_DITHER 中定义的值*/
    uint8_t SLEW_SET;           /*设置 SC8815 反向放电模式下VBUS动态变化的速率-这个参数应取 SCHWI_SLEW_SET 中定义的值*/
    uint8_t ADC_SCAN;           /*打开或关闭 SC8815 内部ADC转换功能 -这个参数应取 SCHWI_ADC_SCAN 中定义的值*/
    uint8_t ILIM_BW;            /*设置 SC8815 限流环路的带宽        -这个参数应取 SCHWI_ILIM_BW 中定义的值*/
    uint8_t LOOP;               /*设置 SC8815 环路响应控制模式      -这个参数应取 SCHWI_LOOP 中定义的值*/
    uint8_t ShortFoldBack;      /*打开或关闭 SC8815 反向放电模式中的 VBUS 短路保护功能-这个参数应取 SCHWI_SFB 中定义的值*/
    uint8_t EOC;                /*设置 SC8815 充电结束的检测闸值    -这个参数应取 SCHWI_EOC 中定义的值*/
    uint8_t PFM;                /*打开或关闭 SC8815 反向放电中轻载下的 PFM 模式-这个参数应取 SCHWI_PFM 中定义的值*/
}HardwareInit;

//SC8815 中断状态结构体定义
typedef struct
{
    uint8_t AC_OK;              /*在 ACIN 引脚处检测到适配器插入中断.*/
    uint8_t INDET;              /*在 INDET 引脚处检测到适配器插入中断.*/
    uint8_t VBUS_SHORT;         /*反向放电模式下检测到 VBUS 短路故障中断.*/
    uint8_t OTP;                /*发生 OTP(过温) 故障中断.*/
    uint8_t EOC;                /*满足 EOC(充电结束) 条件中断.*/
    /*在用作中断配置时,成员设置为 sENABLE(1) 表示打开中断，设置 sDISABLE(0) 为表示关闭中断
    * 在用作获取中断状态时,成员值为 1 表示中断发生,为 0 表示中断未发生.
    * 读取中断状态寄存器后将清除中断状态.*/
}InterruptStatus;

typedef struct{
	uint16_t VBUS_Voltage;			// VBUS 电压
	uint16_t VBUS_Current;			// VBUS 电流
	uint16_t BATT_Voltage;			// 电池电压
	uint16_t BATT_Current;			// 电池电流
	uint16_t ADIN_Voltage;			// ADIN_PIN 电压
	uint16_t OutputVoltage;			// OTG 输出电压
	uint16_t BusCurrentLimit;		// VBUS 限流
	uint16_t BatteryCurrentLimit;	// 电池限流
	uint16_t VINREGVoltage;			// VINREG 电压
	uint16_t MaxOutputVoltage;		// 最大 OTG 可输出电压
	uint16_t OutputVoltageStep;		// OTG 输出电压步进值
	uint8_t VINREGRatio;			// VINREG 的增益
}BatteryStatus;


class SC8815 : public MCU_I2C
{
private:
	BatteryConfig BatteryConfigStruct;
	HardwareInit HardwareInitStruct;
	InterruptStatus InterruptMaskInitStruct;
	BatteryStatus BatteryStatusStruct;

	MCU_GPIO PSTOP;
public:
	void Init(void);

	//SC8815 硬件配置初始化函数
	void BatteryConfig(void);     //配置 SC8815 电池参数 (需要 SC8815 PSTOP 引脚为高才能进行配置)
	void HardwareInit(void);        //初始化 SC8815 硬件配置 (需要 SC8815 PSTOP 引脚为高才能进行配置)
	void ConfigInterruptMask(void);  //配置 SC8815 中断屏蔽 (中断使能或失能)

	//SC8815 读取中断状态函数
	void ReadInterrupStatus(void);

	//SC8815 读取内置ADC转换结果函数
	uint16_t Read_VBUS_Voltage(void);
	uint16_t Read_VBUS_Current(void);
	uint16_t Read_BATT_Voltage(void);
	uint16_t Read_BATT_Current(void);
	uint16_t Read_ADIN_Voltage(void);

	//SC8815 设置参数值函数
	void SetOutputVoltage(uint16_t NewVolt);
	void SetBusCurrentLimit(uint16_t NewILIM);
	void SetBatteryCurrentLimit(uint16_t NewILIM);
	void SetVINREGVoltage(uint16_t NewVolt);

	//SC8815 获取参数设置值函数
	uint16_t GetOutputVoltage(void);
	uint16_t GetBusCurrentLimit(void);
	uint16_t GetBatteryCurrentLimit(void);
	uint16_t GetVINREGVoltage(void);
	uint16_t GetMaxOutputVoltage(void);
	uint16_t GetOutputVoltageStep(void);
	uint8_t GetVINREGRatio(void);

	//SC8815 设置硬件配置函数 (不需要 SC8815 PSTOP 引脚为高)
	void OTG_Enable(void);                       //打开 OTG 反向放电模式
	void OTG_Disable(void);                      //关闭 OTG 反向放电模式, SC8815 恢复到充电模式
	void VINREG_SetRatio_40x(void);              //设置 VINREG 的增益为 40x
	void VINREG_SetRatio_100x(void);             //设置 VINREG 的增益为 100x
	void OVP_Enable(void);                       //打开 OTG 模式中的 VBUS 过压保护功能
	void OVP_Disable(void);                      //关闭 OTG 模式中的 VBUS 过压保护功能
	void PGATE_Enable(void);                     //打开 PGATE 引脚功能, 输出低电平打开 PMOS
	void PGATE_Disable(void);                    //关闭 PGATE 引脚功能, 输出高电平关闭 PMOS
	void GPO_Enable(void);                       //打开 GPO 引脚功能, 输出低电平
	void GPO_Disable(void);                      //关闭 GPO 引脚功能, 输出高阻状态
	void ADC_Enable(void);                       //打开 ADC 扫描, 此时可以读取 ADC 数据
	void ADC_Disable(void);                      //关闭 ADC 扫描, 节约 1-2mA 的耗电
	void SFB_Enable(void);                       //打开 OTG 模式中的 VBUS 和 VBAT 短路保护功能
	void SFB_Disable(void);                      //关闭 OTG 模式中的 VBUS 和 VBAT 短路保护功能
	void PFM_Enable(void);                       //打开 OTG 模式中轻载条件下的 PFM 模式
	void PFM_Disable(void);                      //关闭 OTG 模式中轻载条件下的 PFM 模式

	//SC8815 获取硬件配置状态函数
	uint8_t OTG_IsEnable(void);                  //检查 OTG 是否处于打开状态
	uint8_t OVP_IsEnable(void);                  //检查 OTG 模式中 OVP 功能是否处于打开状态
	uint8_t PGATE_IsEnable(void);                //检查 PGATE 引脚功能是否处于打开状态
	uint8_t GPO_IsEnable(void);                  //检查 GPO 引脚功能是否处于打开状态
	uint8_t ADC_IsEnable(void);                  //检查 ADC 扫描是否处于打开状态
	uint8_t SFB_IsEnable(void);                  //检查 OTG 模式中 短路保护 功能是否处于打开状态
	uint8_t PFM_IsEnable(void);                  //检查 OTG 模式中的 PFM 模式是否处于打开状态

	InterruptStatus GetInterruptMaskInitStruct(void) const { return InterruptMaskInitStruct; };
	BatteryStatus GetBatteryStatusStruct(void) const { return BatteryStatusStruct; };

	SC8815(MCU_GPIO PSTOP, I2C_HandleTypeDef* hi2c, uint8_t SLAVE_ADDRESS = SC8815_ADDR, uint16_t MemAddSize = I2C_MEMADD_SIZE_8BIT);
	~SC8815();
};

#endif