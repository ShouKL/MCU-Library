#include "BQ40ZXX_Class.h"

uint8_t BQ40ZXX::calculate_crc8(uint8_t InitialValue, uint8_t *message ,uint8_t len)		//CRC-8计算 X8+X2+X+1
{
	uint8_t i, crc = InitialValue;		/* 计算后的最终crc值 */

	while(len--)
	{
		crc ^= (*message++);
		for(i = 0;i < 8;i++)
		{
			if(crc & 0x80)
			{
				crc = (crc << 1) ^ 0x07;		//根据多项式计算 CRC
			}
			else crc <<= 1;
		}
	}
	return crc;
}

bool BQ40ZXX::read_word(uint8_t memory_addr, int32_t *readVal)
{
	bq40z50_buf[0] = BQ40Z50_Addr;
	bq40z50_buf[1] = memory_addr;
	bq40z50_buf[2] = BQ40Z50_Addr + 1;
	bq40z50_buf[3] = bq40z50_buf[4] = bq40z50_buf[5] = 0;
    
    if(MCU_I2C::read(&bq40z50_buf[3], 3, memory_addr) == true)	
	{
		if(calculate_crc8(0, bq40z50_buf, 5) == bq40z50_buf[5])
		{
			*readVal = ((int32_t)bq40z50_buf[4]<<8) | bq40z50_buf[3];
			return true;
		}
	}
	else
	{
		HAL_I2C_DeInit(hi2c);
		if (HAL_I2C_Init(hi2c) != HAL_OK)
		{
			Error_Handler();
		}
	}
	return false;
}

bool BQ40ZXX::read_Block(uint16_t memory_addr, uint8_t num, uint8_t *readBuf)
{
	bq40z50_buf[0] = BQ40Z50_Addr;
	bq40z50_buf[1] = 0x44;
	bq40z50_buf[2] = 0x02;
	bq40z50_buf[3] = memory_addr & 0x00FF;
	bq40z50_buf[4] = (memory_addr >> 8) & 0x00FF;
	bq40z50_buf[5] = calculate_crc8(0x00, bq40z50_buf, 5);

  	if(MCU_I2C::write(&bq40z50_buf[2], 4, bq40z50_buf[1]) != true)
	{
		HAL_I2C_DeInit(hi2c);
		if (HAL_I2C_Init(hi2c) != HAL_OK)
		{
			Error_Handler();
		}
		return false;
	}
	
	Delay(25);
	
	bq40z50_buf[0] = BQ40Z50_Addr;
	bq40z50_buf[1] = 0x44;
	bq40z50_buf[2] = BQ40Z50_Addr | 0x01;

 	if(MCU_I2C::read(&bq40z50_buf[3], num + 4, bq40z50_buf[1]) != true)	
	{
		HAL_I2C_DeInit(hi2c);
		if (HAL_I2C_Init(hi2c) != HAL_OK)
		{
			Error_Handler();
		}
		return false;
	}
	else
	{
		if(bq40z50_buf[3] != num + 2)		// 长度加上读取的地址
			return false;

		if(memory_addr != ((bq40z50_buf[5] << 8) | bq40z50_buf[4]))
			return false;
		
		if(calculate_crc8(0, bq40z50_buf, num + 6) != bq40z50_buf[num + 6]) // 校验加上从机地址、寄存器地址长度、地址和校验位
		{
			for(int i = 0;i < num;i++)
			{
				readBuf[i] = bq40z50_buf[i+6];
			}
			return false;
		}
	}
	return true;
}

bool BQ40ZXX::read_cell_voltage(cell_voltage *cell_voltage_data)
{
	uint8_t buf[32];
	if(!read_Block(0x0071, 32, buf))		// 读取第1~4节电池电压
		return false;
	cell_voltage_data->Cell_1 = buf[1]<<8 | buf[0];
	cell_voltage_data->Cell_2 = buf[3]<<8 | buf[2];
	cell_voltage_data->Cell_3 = buf[5]<<8 | buf[4];
	cell_voltage_data->Cell_4 = buf[7]<<8 | buf[6];
	if(!read_Block(0x007B, 18, buf))		// 读取第5~6节电池电压
		return false;
	cell_voltage_data->Cell_5 = buf[1]<<8 | buf[0];	
	cell_voltage_data->Cell_6 = buf[7]<<8 | buf[6];	

	return true;
}

BQ40ZXX::BQ40ZXX(I2C_HandleTypeDef *hi2c, uint8_t SLAVE_ADDRESS, uint16_t MemAddSize) 
                : MCU_I2C(hi2c, SLAVE_ADDRESS, MemAddSize)
{
}

BQ40ZXX::~BQ40ZXX()
{
}
