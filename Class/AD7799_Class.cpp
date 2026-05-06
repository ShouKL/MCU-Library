#include "AD7799_Class.h"

void AD7799::Init(void){
	Reset();
  	Init_Delay(5);
	
	if ((GetRegisterValue(AD7799_REG_ID, 1) & 0x0F) != AD7799_ID) {
		Error_Handler();
	}
}

void AD7799::Reset() {
	uint8_t dataToSend[4] = { 0xff, 0xff, 0xff, 0xff };
	CS.low();
	MCU_SPI::transmit(dataToSend, 4);
	CS.high();
	SetBuffer(1);
}

uint32_t AD7799::GetRegisterValue(uint8_t regAddress, uint8_t size) {
	uint8_t data[4] = { 0x00, 0x00, 0x00, 0x00 };
	uint32_t receivedData = 0x00;
	data[0] = AD7799_COMM_READ | AD7799_COMM_ADDR(regAddress);
	CS.low();
	MCU_SPI::transmit(data, 1);
	MCU_SPI::receive(data, size);
	CS.high();
	if (size == 1) {
		receivedData += (data[0] << 0);
	}
	if (size == 2) {
		receivedData += (data[0] << 8);
		receivedData += (data[1] << 0);
	}
	if (size == 3) {
		receivedData += (data[0] << 16);
		receivedData += (data[1] << 8);
		receivedData += (data[2] << 0);
	}
	return receivedData;
}

void AD7799::SetRegisterValue(uint8_t regAddress, uint32_t regValue, uint8_t size) {
	uint8_t data[4] = { 0x00, 0x00, 0x00, 0x00 };
	data[0] = AD7799_COMM_WRITE | AD7799_COMM_ADDR(regAddress);
	if (size == 1) {
		data[1] = (uint8_t) regValue;
	}
	if (size == 2) {
		data[2] = (uint8_t) ((regValue & 0x0000FF) >> 0);
		data[1] = (uint8_t) ((regValue & 0x00FF00) >> 8);
	}
	if (size == 3) {
		data[3] = (uint8_t) ((regValue & 0x0000FF) >> 0);
		data[2] = (uint8_t) ((regValue & 0x00FF00) >> 8);
		data[1] = (uint8_t) ((regValue & 0xFF0000) >> 16);
		}
	CS.low();
	MCU_SPI::transmit(data, 1 + size);
	CS.high();
}

uint8_t AD7799::Ready() {
	uint8_t rdy = 0;
	rdy = (GetRegisterValue(AD7799_REG_STAT, 1) & 0x80);
	return (!rdy);
}

void AD7799::SetMode(AD7799_Mode mode) {
	uint32_t command;
	command = GetRegisterValue(AD7799_REG_MODE, 2);
	command &= ~AD7799_MODE_SEL(0xFF);
	command |= AD7799_MODE_SEL((uint32_t ) mode);
	SetRegisterValue(AD7799_REG_MODE, command, 2);
	m_mode = mode;
}

void AD7799::SetRate(AD7799_Rate rate) {
	uint32_t command;
	command = GetRegisterValue(AD7799_REG_MODE, 2);
	command &= ~AD7799_MODE_RATE(0xFF);
	command |= AD7799_MODE_RATE((uint32_t ) rate);
	SetRegisterValue(AD7799_REG_MODE, command, 2);
	m_rate = rate;
}

void AD7799::SetGain(AD7799_Gain gain) {
    uint32_t command = GetRegisterValue(AD7799_REG_CONF, 2);
    command &= ~(0x07 << 8); // 明确清除 Bit 8, 9, 10
    command |= ((uint32_t)gain << 8);
    SetRegisterValue(AD7799_REG_CONF, command, 2);
    m_gain = gain;
}

void AD7799::SetChannel(AD7799_Channel channel) {
    uint32_t command = GetRegisterValue(AD7799_REG_CONF, 2);
    command &= ~0x07; // 明确清除低 3 位 (Channel 位)
    command |= ((uint32_t)channel & 0x07);
    SetRegisterValue(AD7799_REG_CONF, command, 2);
    m_channel = channel;
}

void AD7799::SetReference(uint8_t state) {
	uint32_t command = 0;
	command = GetRegisterValue(AD7799_REG_CONF, 2);
	command &= ~AD7799_CONF_REFDET(1);
	command |= AD7799_CONF_REFDET(state);
	SetRegisterValue(AD7799_REG_CONF, command, 2);
}

void AD7799::SetPolarity(AD7799_Polarity polarity) {
	uint32_t command = 0;
	command = GetRegisterValue(AD7799_REG_CONF, 2);
	command &= ~AD7799_CONF_POLAR(1);
	command |= AD7799_CONF_POLAR((uint32_t ) polarity);
	SetRegisterValue(AD7799_REG_CONF, command, 2);
	m_polarity = polarity;
}

void AD7799::SetBuffer(uint8_t state) {
    uint16_t command = (uint16_t)GetRegisterValue(AD7799_REG_CONF, 2);
    if (state) {
        command |= (1 << 4);  // 置位 Bit 4
    } else {
        command &= ~(1 << 4); // 清零 Bit 4
    }
    SetRegisterValue(AD7799_REG_CONF, command, 2);
}

HAL_StatusTypeDef AD7799::SingleConversion() {
	SetMode(AD7799_MODE_SINGLE);

	uint32_t startTime = HAL_GetTick();
	while (!Ready()) {
		if (HAL_GetTick() - startTime > (uint32_t) (1.5 * settle_time_ms[(uint8_t) m_rate])) {
			return HAL_TIMEOUT;
		}
	}
	rawConversion = GetRegisterValue(AD7799_REG_DATA, 3);

	RawToVolt();

	return HAL_OK;
}

void AD7799::RawToVolt() {
    uint32_t actual_gain = 1 << ((uint8_t)m_gain & 0x07);
    
    double dvref = (double)vref;
    double dgain = (double)actual_gain;
    double raw = (double)rawConversion;

    if (m_polarity == AD7799_UNIPOLAR) {
        voltConversion = (float)((raw * dvref) / (16777216.0 * dgain));
    } else {
        voltConversion = (float)(((raw - 8388608.0) * dvref) / (8388608.0 * dgain));
    }
}

uint8_t AD7799::CheckConnection(void) {
    uint32_t original_conf = GetRegisterValue(AD7799_REG_CONF, 2);
    SetRegisterValue(AD7799_REG_CONF, original_conf | AD7799_CONF_BO_EN, 2);

    SetMode(AD7799_MODE_SINGLE);
    uint32_t start = HAL_GetTick();
    while(!Ready()) {
        if(HAL_GetTick() - start > 500) break;
    }

    uint8_t stat = (uint8_t)GetRegisterValue(AD7799_REG_STAT, 1);
    uint32_t raw = GetRegisterValue(AD7799_REG_DATA, 3);

    SetRegisterValue(AD7799_REG_CONF, original_conf, 2);

    if ((stat & AD7799_STAT_ERR) || (raw >= 0xFFFFEE)) {
        return 1;
    }
    return 0;
}

AD7799::AD7799(SPI_HandleTypeDef* hspi, MCU_GPIO cs, float vref, AD7799_Rate rate, AD7799_Mode mode, AD7799_Channel channel, 
	AD7799_Gain gain, AD7799_Polarity polarity, uint32_t rawConversion, float voltConversion) 
			: MCU_SPI(hspi, cs), vref(vref), m_rate(rate), m_mode(mode), m_channel(channel), m_gain(gain), 
			   m_polarity(polarity), rawConversion(rawConversion), voltConversion(voltConversion)
{
}

AD7799::~AD7799()
{
}

#endif
