/*
 * ad7768.c
 *
 *  Created on: Feb 25, 2021
 *      Author: ROTA0101
 */

#include "ad7768.h"
#include "debug.h"

extern SPI_HandleTypeDef hspi3;

void AD7768_writeReg(uint8_t addr, uint8_t value)
{
	uint8_t buf[2];
	uint8_t read[2];

	buf[0] = (addr & 0x7F);
	buf[1] = value;
	HAL_GPIO_WritePin(ADC_CS_PIN, ADC_CS_PIN_NO, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi3, &buf[0], &read[0], 2, 1000);
	HAL_GPIO_WritePin(ADC_CS_PIN, ADC_CS_PIN_NO, GPIO_PIN_SET);
}


uint8_t AD7768_readReg(uint8_t addr)
{
	uint8_t buf[2];
	uint8_t read[2];
	HAL_GPIO_WritePin(ADC_CS_PIN, ADC_CS_PIN_NO, GPIO_PIN_RESET);
	buf[0] = 0x80 | (addr & 0x7F);
	buf[1] = 0x00;
	HAL_SPI_TransmitReceive(&hspi3, &buf[0], &read[0], 2, 1000);
	HAL_GPIO_WritePin(ADC_CS_PIN, ADC_CS_PIN_NO, GPIO_PIN_SET);

	HAL_GPIO_WritePin(ADC_CS_PIN, ADC_CS_PIN_NO, GPIO_PIN_RESET);
	buf[0] = 0x80 | (addr & 0x7F);
	buf[1] = 0x00;
	HAL_SPI_TransmitReceive(&hspi3, &buf[0], &read[0], 2, 1000);
	HAL_GPIO_WritePin(ADC_CS_PIN, ADC_CS_PIN_NO, GPIO_PIN_SET);

	return read[1];
}


void AD7768_setup(uint8_t dClk, uint8_t decRate, uint8_t pwrMode)
{
	uint8_t readVal = AD7768_readReg(AD7768_REG_REV_ID);

	dbg_printf("readVal : %x  \n", readVal);
	if (readVal != 0x06)
	{
		dbg_printf("There is a problem with ADC\n");
	}HAL_Delay(100);

	AD7768_writeReg(AD7768_REG_CH_MODE_A, decRate);
	HAL_Delay(100);
	AD7768_writeReg(AD7768_REG_PWR_MODE, pwrMode);
	HAL_Delay(100);
	AD7768_writeReg(AD7768_REG_INTERFACE_CFG, dClk);
	HAL_Delay(100);

	dbg_printf("decRate : %x - pwrMode : %x dClk : %x\n", decRate, pwrMode, dClk);
}
