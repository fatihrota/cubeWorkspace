/*
 * ltc2983.c
 *
 *  Created on: Feb 5, 2021
 *      Author: ROTA0101
 */

#include "ltc2983.h"
#include "debug.h"

uint8_t writeCmd = 0x02;
uint8_t readCmd = 0x03;

extern SPI_HandleTypeDef hspi1;


void LTC_writeReg8(uint16_t addr, uint8_t value)
{
	uint8_t addrArr[2];
	addrArr[1] = addr & 0xFF;
	addrArr[0] = addr >> 8;


	HAL_GPIO_WritePin(LTC_CS_PIN, LTC_CS_PIN_NO, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &writeCmd, 1, 1000);
	HAL_SPI_Transmit(&hspi1, &addrArr[0], 2, 1000);
	HAL_SPI_Transmit(&hspi1, &value, 1, 1000);
	HAL_GPIO_WritePin(LTC_CS_PIN, LTC_CS_PIN_NO, GPIO_PIN_SET);

}

void LTC_writeReg32(uint16_t addr, uint32_t value)
{
	uint8_t addrArr[2];
	addrArr[1] = addr & 0xFF;
	addrArr[0] = addr >> 8;

	uint8_t dataArray[4];
	dataArray[3] = (uint8_t)value;
	dataArray[2] = (uint8_t)(value>>=8);
	dataArray[1] = (uint8_t)(value>>=8);
	dataArray[0] = (uint8_t)(value>>=8);

	HAL_GPIO_WritePin(LTC_CS_PIN, LTC_CS_PIN_NO, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &writeCmd, 1, 1000);
	HAL_SPI_Transmit(&hspi1, &addrArr[0], 2, 1000);
	HAL_SPI_Transmit(&hspi1, &dataArray[0], 4, 1000);
	HAL_GPIO_WritePin(LTC_CS_PIN, LTC_CS_PIN_NO, GPIO_PIN_SET);

}

uint8_t LTC_readReg8(uint16_t addr)
{
	uint8_t addrArr[2];
	addrArr[1] = addr & 0xFF;
	addrArr[0] = addr >> 8;

	uint8_t readVal = 0;

	HAL_GPIO_WritePin(LTC_CS_PIN, LTC_CS_PIN_NO, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &readCmd, 1, 1000);
	HAL_SPI_Transmit(&hspi1, &addrArr[0], 2, 1000);
	HAL_SPI_Receive(&hspi1, &readVal, 1, 1000);
	HAL_GPIO_WritePin(LTC_CS_PIN, LTC_CS_PIN_NO, GPIO_PIN_SET);

	return readVal;
}

uint32_t LTC_readReg32(uint16_t addr)
{
	uint8_t addrArr[2];
	addrArr[1] = addr & 0xFF;
	addrArr[0] = addr >> 8;

	uint8_t readVal[4];

	HAL_GPIO_WritePin(LTC_CS_PIN, LTC_CS_PIN_NO, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &readCmd, 1, 1000);
	HAL_SPI_Transmit(&hspi1, &addrArr[0], 2, 1000);
	HAL_SPI_Receive(&hspi1, &readVal, 4, 1000);
	HAL_GPIO_WritePin(LTC_CS_PIN, LTC_CS_PIN_NO, GPIO_PIN_SET);

	uint32_t rawData = (((uint32_t) readVal[0]) << 24 |
					  ((uint32_t) readVal[1]) << 16 |
					  ((uint32_t) readVal[2]) <<  8 |
					  ((uint32_t) readVal[3]) <<  0);

	return rawData;
}

void LTC_waitUntilFinish(void)
{
	uint8_t process_fin = 0;

	while(process_fin == 0)
	{
		uint8_t status = LTC_readReg8(COMMAND_STATUS_REGISTER);

		process_fin = status & 0x40;
	}
}

void LTC_beginConv(uint8_t ch)
{
	uint8_t beginConv = 0x80 | ch;
	LTC_writeReg8(COMMAND_STATUS_REGISTER, beginConv);

	LTC_waitUntilFinish();
}

void LTC_readTemperature(void)
{
	for (int idx = 0; idx < 20; idx++)
	{
		uint32_t rawTemperature = LTC_readReg32(READ_CH_BASE + 4 * idx);
		uint8_t flag = rawTemperature >> 24;

		uint32_t rawConvData = rawTemperature & 0xFFFFFF;

		int signedRawData = rawConvData;

		if(signedRawData & 0x800000)
			signedRawData = signedRawData | 0xFF000000;

		float temperature = (float)signedRawData / 1024;

		dbg_printf("Ch : %d - Temperature : %f status : %x - %d\n", idx+1, temperature, flag);
		HAL_Delay(100);
	}
}

void LTC_TC_configureChannel(uint32_t ch, uint32_t tcType, uint32_t cjChannel, uint32_t singleEnded, uint32_t openCktDetect, uint32_t openCktDetectCurrent)
{
	uint32_t chCfgData = 0;
	chCfgData = tcType | cjChannel | singleEnded | openCktDetect | openCktDetectCurrent;

	LTC_writeReg32(CH_ADDRESS_BASE + 4*ch, chCfgData);
}

void LTC_RTD_configureChannel(uint32_t ch, uint32_t rtdSensorType, uint32_t rSenseChannel, uint32_t wireNo, uint32_t excitationMode, uint32_t excitationCurrent, uint32_t rtdStandard)
{
	uint32_t rtdData = rtdSensorType | rSenseChannel | wireNo | excitationMode | excitationCurrent | rtdStandard;

	LTC_writeReg32(CH_ADDRESS_BASE + 0x2c, rtdData);

}

void LTC_RTD_configureRSense(uint32_t ch, uint32_t sensorType, uint32_t rSenseValHigh, uint32_t rSenseValLow)
{
	uint32_t rSenseData = sensorType | rSenseValHigh | rSenseValLow;

	LTC_writeReg32(CH_ADDRESS_BASE + 4 * ch, rSenseData);
}

void LTC_init(void)
{
	uint8_t status = LTC_readReg8(COMMAND_STATUS_REGISTER);
	dbg_printf("Initial status : %x\n", status);

	uint32_t readMulti = 0;

	for (int idx = 0; idx < 8; idx++)
	{
		LTC_TC_configureChannel(idx, SENSOR_TYPE__TYPE_T_THERMOCOUPLE, TC_COLD_JUNCTION_CH__12,
										TC_SINGLE_ENDED, TC_OPEN_CKT_DETECT__YES, TC_OPEN_CKT_DETECT_CURRENT__10UA);
		readMulti |= (1 << idx);
	}

	for (int idx = 12; idx < 20; idx++)
	{
		LTC_TC_configureChannel(idx, SENSOR_TYPE__TYPE_T_THERMOCOUPLE, TC_COLD_JUNCTION_CH__12,
										TC_SINGLE_ENDED, TC_OPEN_CKT_DETECT__YES, TC_OPEN_CKT_DETECT_CURRENT__10UA);
		readMulti |= (1 << idx);
	}

	LTC_writeReg32(MULTI_READ_CH_REGISTER, readMulti);

	LTC_RTD_configureChannel(11, SENSOR_TYPE__RTD_PT_100, RTD_RSENSE_CHANNEL__10, RTD_NUM_WIRES__4_WIRE,
							RTD_EXCITATION_MODE__NO_ROTATION_NO_SHARING, RTD_EXCITATION_CURRENT__25UA, RTD_STANDARD__EUROPEAN);


	LTC_RTD_configureRSense(9, SENSOR_TYPE__SENSE_RESISTOR, RSENSE_2K, RSENSE_03K);


	HAL_Delay(1000);

}
