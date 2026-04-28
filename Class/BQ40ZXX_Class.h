#ifndef __BQ40ZXX_CLASS_H__
#define __BQ40ZXX_CLASS_H__

#include "user_main.h"
#include "I2C_Class.h"

#include <stdint.h>

#define BQ40Z50_Addr       0x16    // BQ40Z50 I2C地址（7位）

typedef enum
{
	temperature = 0x08,		//读取电池温度 单位0.1℃
	voltage = 0x09,			//读取电池组电压	单位：mv
	current = 0x0A,			//读取电池电流	单位：mA
	capacity = 0x0D,		//读取电池电量	单位：%
	remainingCapacity = 0x0F,	//读取电池剩余容量	单位：mah
	fullChargeCapacity = 0x10,	//读取电池充满电的容量 单位：mah
	runTimeToEmpty = 0x11,		//读取根据当前放电速率预测的剩余放电时间 单位：min	 65535 = 电池未放电
	averageTimeToEmpty = 0x12,	//读取根据当前平均放电速率预测的剩余放电时间 单位：min	 65535 = 电池未放电
	averageTimeToFull = 0x13,	//读取根据当前充电速率预测平均到满的时间 单位：min	 65535 = 电池未充电
	chargingCurrent = 0x14,		////获取电池支持的充电电流	单位：mA
	chargingVoltage = 0x15,		//获取电池支持的充电电压	单位：mV
	batteryStatusDSG = 0x16,	//读取电池充放电状态	1：电池处于放电或闲置模式  0：电池处于充电模式
	cycleCount = 0x17,		//读取电池循环次数
	designCapacity = 0x18,		//读取电池设计容量
	designVoltage = 0x19,		//读取电池设计电压	单位：mV
	relativeStateOfCharge = 0x4F,	//读取电池健康值	单位：%
}bq40zxxReg;


typedef struct{
	uint16_t Cell_1;
	uint16_t Cell_2;
	uint16_t Cell_3;
	uint16_t Cell_4;
	uint16_t Cell_5;
	uint16_t Cell_6;
}cell_voltage;

class BQ40ZXX : public MCU_I2C
{
	private:
		uint8_t bq40z50_buf[40];
		cell_voltage cell_voltage_data;

	public:
	    uint8_t calculate_crc8(uint8_t InitialValue, uint8_t *message ,uint8_t len);		//CRC-8计算 X8+X2+X+1
		
		bool read_word(uint8_t memory_addr, int32_t *readVal);		//读取一个字
		bool read_Block(uint16_t memory_addr, uint8_t num, uint8_t *readBuf);
		bool read_cell_voltage(cell_voltage *cell_voltage_data);

		BQ40ZXX(I2C_HandleTypeDef* hi2c, uint8_t SLAVE_ADDRESS = BQ40Z50_Addr, uint16_t MemAddSize = I2C_MEMADD_SIZE_8BIT);
		~BQ40ZXX();

};

#endif
