/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2019 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        ltc298x.c
 * @brief       LTC Temperature measurement card
 *
 * @author		Mehmet Fatih Özay
 * @date		11-02-2021
 *
 * @ingroup     vts
 * @{
 *****************************************************************************/

/*============================================================================*/
/* Includes                                                                   */
/*============================================================================*/

/*============================================================================*/
/* Forward declarations                                                       */
/*============================================================================*/

/*============================================================================*/
/* Constants and macros                                                       */
/*============================================================================*/

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/

#include "ltc2983.h"
#include "leds.h"
#include "rt_config.h"
#include <string.h>
#include <math.h>
#include "conf.h"
#include "debug.h"
#include "gpio.h"

uint8_t writeCmd = 0x02;
uint8_t readCmd = 0x03;

extern SPI_HandleTypeDef hspi1;


/*****************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	if (GPIO_Pin == GPIO_PIN_11)
	{
		//dbg_printf("INT\r\n");
		//LTC_readTemperatureAll();
	}


}

/*****************************************************************************/
void LTC_writeReg8(uint16_t addr, uint8_t value)
{
	uint8_t addrArr[2];
	addrArr[1] = addr & 0xFF;
	addrArr[0] = addr >> 8;


	HAL_GPIO_WritePin(LTC_CS_PORT, LTC_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &writeCmd, 1, 1000);
	HAL_SPI_Transmit(&hspi1, &addrArr[0], 2, 1000);
	HAL_SPI_Transmit(&hspi1, &value, 1, 1000);
	HAL_GPIO_WritePin(LTC_CS_PORT, LTC_CS_PIN, GPIO_PIN_SET);

}

/*****************************************************************************/
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

	HAL_GPIO_WritePin(LTC_CS_PORT, LTC_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &writeCmd, 1, 1000);
	HAL_SPI_Transmit(&hspi1, &addrArr[0], 2, 1000);
	HAL_SPI_Transmit(&hspi1, &dataArray[0], 4, 1000);
	HAL_GPIO_WritePin(LTC_CS_PORT, LTC_CS_PIN, GPIO_PIN_SET);

}

/*****************************************************************************/
uint8_t LTC_readReg8(uint16_t addr)
{
	uint8_t addrArr[2];
	addrArr[1] = addr & 0xFF;
	addrArr[0] = addr >> 8;

	uint8_t readVal = 0;

	HAL_GPIO_WritePin(LTC_CS_PORT, LTC_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &readCmd, 1, 1000);
	HAL_SPI_Transmit(&hspi1, &addrArr[0], 2, 1000);
	HAL_SPI_Receive(&hspi1, &readVal, 1, 1000);
	HAL_GPIO_WritePin(LTC_CS_PORT, LTC_CS_PIN, GPIO_PIN_SET);

	return readVal;
}

/*****************************************************************************/
uint32_t LTC_readReg32(uint16_t addr)
{
	uint8_t addrArr[2];
	addrArr[1] = addr & 0xFF;
	addrArr[0] = addr >> 8;

	uint8_t readVal[4];

	HAL_GPIO_WritePin(LTC_CS_PORT, LTC_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &readCmd, 1, 1000);
	HAL_SPI_Transmit(&hspi1, &addrArr[0], 2, 1000);
	HAL_SPI_Receive(&hspi1, &readVal, 4, 1000);
	HAL_GPIO_WritePin(LTC_CS_PORT, LTC_CS_PIN, GPIO_PIN_SET);

	uint32_t rawData = (((uint32_t) readVal[0]) << 24 |
					  ((uint32_t) readVal[1]) << 16 |
					  ((uint32_t) readVal[2]) <<  8 |
					  ((uint32_t) readVal[3]) <<  0);

	return rawData;
}

/*****************************************************************************/
void LTC_waitUntilFinish(void)
{
	uint8_t process_fin = 0;
	uint8_t counter = 0;

	while(process_fin == 0)
	{
		uint8_t status = LTC_readReg8(COMMAND_STATUS_REGISTER);

		process_fin = status & 0x40;

		if (counter%2)
		{
			leds_1_on();
			HAL_Delay(10);
		}
		else
		{
			leds_1_off();
			HAL_Delay(10);
		}
		counter++;
	}
}

/*****************************************************************************/
void LTC_enterSleepMode()
{
	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
	 //HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
	 NVIC_SystemReset();

	//LTC_sleep();

	//uint8_t status = LTC_readReg8(COMMAND_STATUS_REGISTER);
	//LTC_init();
	/*while (status == 0)
	{
		status = LTC_readReg8(COMMAND_STATUS_REGISTER);

	}

	LTC_init();*/

	//NVIC_SystemReset();
}

/*****************************************************************************/
void LTC_beginConv(uint8_t ch)
{
	uint8_t beginConv = 0x80 | ch;
	LTC_writeReg8(COMMAND_STATUS_REGISTER, beginConv);

	LTC_waitUntilFinish();
}

float LTC_readTemperature(uint32_t ch)
{
	uint32_t rawTemperature = LTC_readReg32(READ_CH_BASE + 4 * ch);

	uint8_t flag = rawTemperature >> 24;

	int32_t signedRawData = rawTemperature & 0xFFFFFF;

	if(signedRawData & 0x800000)
		signedRawData = signedRawData | 0xFF000000;

	float temperature = (float)signedRawData / 1024;


	chData[ch].HJ = temperature - gConfig.ch_conf[ch].cjOffset.cjOff;
	chData[ch].CJ = chData[11].HJ;
	chData[ch].flag = flag;

	return temperature;
}

/*****************************************************************************/
void LTC_readTemperatureAll(void)
{
	for (int idx = 0; idx < 20; idx++)
	{
		float temperature = LTC_readTemperature(idx);
		/*dbg_printf("Ch : %d - Temperature : %f\n", idx+1, temperature);
		HAL_Delay(100);*/
	}
}

/*****************************************************************************/
void LTC_reportFahrenheit(void)
{
	uint8_t tmp = LTC_readReg8(GLOBAL_CONFIG_REGISTER);
	LTC_writeReg8(GLOBAL_CONFIG_REGISTER, tmp | 0x04);
}

/*****************************************************************************/
void LTC_reportCelcius(void)
{
	uint8_t tmp = LTC_readReg8(GLOBAL_CONFIG_REGISTER);
	LTC_writeReg8(GLOBAL_CONFIG_REGISTER, tmp & ~0x04);
}

/*****************************************************************************/
void LTC_reject5060Hz(void)
{
	uint8_t tmp = LTC_readReg8(GLOBAL_CONFIG_REGISTER) & ~0x03;
	LTC_writeReg8(GLOBAL_CONFIG_REGISTER, tmp | REJECTION__50_60_HZ);
}

/*****************************************************************************/
void LTC_reject60Hz(void)
{
	uint8_t tmp = LTC_readReg8(GLOBAL_CONFIG_REGISTER) & ~0x03;
	LTC_writeReg8(GLOBAL_CONFIG_REGISTER, tmp | REJECTION__60_HZ);
}

/*****************************************************************************/
void LTC_reject50Hz(void)
{
	uint8_t tmp = LTC_readReg8(GLOBAL_CONFIG_REGISTER) & ~0x03;
	LTC_writeReg8(GLOBAL_CONFIG_REGISTER, tmp | REJECTION__50_HZ);
}

/*****************************************************************************/
void LTC_enableConvChannels(uint32_t ch)
{
	LTC_writeReg32(MULTI_READ_CH_REGISTER, ch);
}

/*****************************************************************************/
void LTC_sleep(void)
{
	LTC_writeReg8(COMMAND_STATUS_REGISTER, LTC_CMD_SLEEP);
}

/*****************************************************************************/
void LTC_disableChannel(uint8_t ch)
{
	LTC_writeReg32(CH_ADDRESS_BASE + 4 * ch, 0);
}

/*****************************************************************************/
float LTC_readADC(uint8_t ch)
{
	uint32_t rawData = LTC_readReg32(READ_CH_BASE + 4 * ch);

	uint8_t flag = rawData >> 24;

	int32_t signedRawData = rawData & 0xFFFFFF;

	if(signedRawData & 0x800000)
		signedRawData = signedRawData | 0xFF000000;

	float adcValue = (float)signedRawData / 2097152.0;

	return adcValue;
}

/*****************************************************************************/
void LTC_TC_configureChannel(uint32_t ch, uint32_t tcType, uint32_t cjChannel, uint32_t singleEnded, uint32_t openCktDetect, uint32_t openCktDetectCurrent)
{
	uint32_t chCfgData = 0;
	chCfgData = tcType | cjChannel | singleEnded | openCktDetect | openCktDetectCurrent;

	chConfig[ch].config = tcType;

	dbg_printf("Config Int : %d\n", chConfig[ch].config);

	LTC_writeReg32(CH_ADDRESS_BASE + 4*ch, chCfgData);
}


/*****************************************************************************/
void LTC_RTD_configureChannel(uint32_t ch, uint32_t rtdSensorType, uint32_t rSenseChannel, uint32_t wireNo, uint32_t excitationMode, uint32_t excitationCurrent, uint32_t rtdStandard)
{
	uint32_t rtdData = rtdSensorType | rSenseChannel | wireNo | excitationMode | excitationCurrent | rtdStandard;

	LTC_writeReg32(CH_ADDRESS_BASE + 4 * ch, rtdData);

}

/*****************************************************************************/
void LTC_RTD_configureRSense(uint32_t ch, uint32_t sensorType, uint32_t rSenseValHigh, uint32_t rSenseValLow)
{
	uint32_t rSenseData = sensorType | rSenseValHigh | rSenseValLow;

	LTC_writeReg32(CH_ADDRESS_BASE + 4 * ch, rSenseData);
}

/*****************************************************************************/
int counter = 0;
void LTC_process(void)
{
	LTC_beginConv(0);
	LTC_readTemperatureAll();
	if (counter % 10 == 0)
	{
		for (int idx = 0; idx < 20; idx++)
		{
			dbg_printf("Ch : %d - Temperature : %f\r\n", idx+1, chData[idx].HJ);
			HAL_Delay(10);
		}
	}
	if (counter%2)
	{
		leds_1_on();
	}
	else
	{
		leds_1_off();
	}
	counter++;
}

/*****************************************************************************/
void LTC_init(void)
{
	uint8_t status = LTC_readReg8(COMMAND_STATUS_REGISTER);
	dbg_printf("Initial status : %x\n", status);

	uint32_t readMulti = 0;

	for (int idx = 0; idx < 8; idx++)
	{
		/*LTC_TC_configureChannel(idx, SENSOR_TYPE__TYPE_T_THERMOCOUPLE, TC_COLD_JUNCTION_CH__12,

										TC_SINGLE_ENDED, TC_OPEN_CKT_DETECT__YES, TC_OPEN_CKT_DETECT_CURRENT__10UA);*/

		LTC_TC_configureChannel(idx, gConfig.ch_conf[idx].sensorType, gConfig.ch_conf[idx].CJ_channel,
								gConfig.ch_conf[idx].singleEnded, gConfig.ch_conf[idx].openCktDetect, gConfig.ch_conf[idx].openCktDetectCurrent);

		dbg_printf("gConfig.ch_conf[%d].enabled : %d\n", idx, gConfig.ch_conf[idx].enabled);
		if (gConfig.ch_conf[idx].enabled)
			readMulti |= (1 << idx);

	}

	for (int idx = 12; idx < 20; idx++)
	{
		/*LTC_TC_configureChannel(idx, SENSOR_TYPE__TYPE_T_THERMOCOUPLE, TC_COLD_JUNCTION_CH__12,
										TC_SINGLE_ENDED, TC_OPEN_CKT_DETECT__YES, TC_OPEN_CKT_DETECT_CURRENT__10UA);*/
		LTC_TC_configureChannel(idx, gConfig.ch_conf[idx].sensorType, gConfig.ch_conf[idx].CJ_channel,
										gConfig.ch_conf[idx].singleEnded, gConfig.ch_conf[idx].openCktDetect, gConfig.ch_conf[idx].openCktDetectCurrent);

		dbg_printf("gConfig.ch_conf[%d].enabled : %d\n", idx, gConfig.ch_conf[idx].enabled);

		if (gConfig.ch_conf[idx].enabled)
			readMulti |= (1 << idx);
	}

	LTC_enableConvChannels(readMulti);

	LTC_RTD_configureChannel(RTD_CHANNEL, SENSOR_TYPE__RTD_PT_100, RTD_RSENSE_CHANNEL__10, RTD_NUM_WIRES__4_WIRE,
							RTD_EXCITATION_MODE__NO_ROTATION_NO_SHARING, RTD_EXCITATION_CURRENT__25UA, RTD_STANDARD__EUROPEAN);


	LTC_RTD_configureRSense(RSENSE_CHANNEL, SENSOR_TYPE__SENSE_RESISTOR, RSENSE_2K, RSENSE_03K);


	HAL_Delay(1000);

}



/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2019,  All Rights Reserved. Confidential.      */
/******************************************************************************/
