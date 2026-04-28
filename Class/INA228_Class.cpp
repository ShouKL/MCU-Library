#include "INA228_Class.h"

void INA228::Init(void)
{
    uint16_t data;
	data = 1<<15;
	uint8_t data0[2] = {(uint8_t)(data>>8), (uint8_t)data};
	MCU_I2C::write(data0, 2, INA228_CONFIG);

	CURRENT_LSB = m_INA228_Config->MEC/524288.0;
	data = m_INA228_Config->RST<<15|m_INA228_Config->RSTACC<<14|m_INA228_Config->CONVDLY<<6|
            m_INA228_Config->TEMPCOMP<<5|m_INA228_Config->ADCRANGE<<4;

	uint8_t data1[2]={(uint8_t)(data>>8),(uint8_t)data};
	MCU_I2C::write(data1, 2, INA228_CONFIG);

    data = m_INA228_ADC->MODE<<12|m_INA228_ADC->VBUSCT<<9|
                    m_INA228_ADC->VSHCT<<6|m_INA228_ADC->VTCT<<3|
                    m_INA228_ADC->AVG;
	uint8_t data2[2]={(uint8_t)(data>>8),(uint8_t)data};
	MCU_I2C::write(data2, 2, INA228_ADC_CONFIG);
}

//分流电阻校准，单位：毫欧,ppm/℃
void INA228::Set_SHUNT_CAL(float RSHUNT,uint16_t ppm){
	uint16_t SHUNT_CAL;
	if(m_INA228_Config->ADCRANGE == 1)
		SHUNT_CAL = 131072  * CURRENT_LSB * 100000 * (RSHUNT/1000.0) * 4;
	else
	  SHUNT_CAL = 131072  * CURRENT_LSB * 100000 * (RSHUNT/1000.0);
	
	uint8_t data1[2]={(uint8_t)(SHUNT_CAL>>8),(uint8_t)SHUNT_CAL};
    MCU_I2C::write(data1, 2, INA228_SHUNT_CAL);
	uint8_t data2[2]={(uint8_t)(ppm>>8),(uint8_t)ppm};
	MCU_I2C::write(data2, 2, INA228_SHUNT_TEMPCO);
}


//获取分流电阻电压
float INA228::Get_VSHUNT(void){
	float VSHUNT;
//	uint32_t data;
	uint8_t data1[3];
    MCU_I2C::read(data1, 3, INA228_VSHUNT);
	int32_t data = (data1[0]<<16|data1[1]<<8|data1[2])>>4;
	if(data  & 0x80000){
		data |= 0xFFF00000;
	}
//	data = data>>4;
	if(m_INA228_Config->ADCRANGE == 1)
		VSHUNT = 0.0000003125*data;
	else
		VSHUNT = 0.000000078125*data;
	return VSHUNT;
}

//获取VBUS电压
float INA228::Get_VBUS(void){
	float VBUS;
	uint8_t data1[3];
    MCU_I2C::read(data1, 3, INA228_VBUS);
	uint32_t data = (data1[0]<<16|data1[1]<<8|data1[2])>>4;
	VBUS = 0.0001953125*data;
	return VBUS;
}

//获取芯片温度
float INA228::Get_DIETEMP(void){
	float TEMP;
  int16_t data;
	uint8_t data1[2];
    MCU_I2C::read(data1, 2, INA228_DIETEMP);
	data = data1[0]<<8|data1[1];
	
	TEMP = 0.0078125*data;
	return TEMP;
}


//获取电流
float INA228::Get_CURRENT(void){
	float CURRENT;
	uint8_t data1[3];
    MCU_I2C::read(data1, 3, INA228_CURRENT);
	int32_t data = (data1[0]<<16|data1[1]<<8|data1[2])>>4;
	if(data  & 0x80000){
		data |= 0xFFF00000;
	}

	CURRENT = data*CURRENT_LSB;
	return CURRENT;
}

//获取功率
float INA228::Get_POWER(void){
	float POWER;
	uint8_t data1[3];
    MCU_I2C::read(data1, 3, INA228_POWER);
	uint32_t data = data1[0]<<16|data1[1]<<8|data1[2];
	POWER = data*CURRENT_LSB*3.2;
	return POWER;
}

//获取能量
float INA228::Get_ENERGY(void){
	float ENERGY;
	uint64_t data;
	uint8_t data1[5];
    MCU_I2C::read(data1, 5, INA228_ENERGY);
	data = data1[0];
	data = data<<32;
    data = data1[1]<<24|data1[2]<<16|data1[1]<<8|data1[0];
	ENERGY = 16.0*3.2*CURRENT_LSB*data;
	return ENERGY;
}

//获取电荷量
float INA228::Get_CHARGE(void){
	float CHARGE;
	int64_t data;
	uint8_t data1[5];
    MCU_I2C::read(data1, 5, INA228_CHARGE);
	data = data1[0];
	data = data<<32;
    data = data1[1]<<24|data1[2]<<16|data1[1]<<8|data1[0];
	CHARGE = CURRENT_LSB*data;
	return CHARGE;
}

//将累积寄存器 ENERGY 和 CHARGE 的内容复位为 0
void INA228::RESET_ACC(void){
	uint16_t data = m_INA228_Config->RST<<15|m_INA228_Config->RSTACC<<14|
                m_INA228_Config->CONVDLY<<6|m_INA228_Config->TEMPCOMP<<5|m_INA228_Config->ADCRANGE<<4;
	data |= 0x4000;
    uint8_t data1[2]={(uint8_t)(data>>8),(uint8_t)data};
    MCU_I2C::write(data1, 2, INA228_CONFIG);
}

INA228::INA228(I2C_HandleTypeDef* hi2c, INA228_Config *INA228_ConfigInit, INA228_ADC *INA228_ADCInit, 
		INA228_ALERT *INA228_AlertInit,uint8_t SLAVE_ADDRESS, uint16_t MemAddSize)
        :   MCU_I2C(hi2c, SLAVE_ADDRESS, MemAddSize),
            m_INA228_Config(INA228_ConfigInit), m_INA228_ADC(INA228_ADCInit),
            m_INA228_Alert(INA228_AlertInit)
{
}

INA228::~INA228()
{
}