/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2019 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        MAX31856.h
 * @brief       MAX31856 chip implementation.
 *
 * @author      LeventK
 * @date        2017-07-31
 *
 * @ingroup     vts
 * @{
 *****************************************************************************/

/*============================================================================*/
/* Includes                                                                   */
/*============================================================================*/

#include "main.h"

/*============================================================================*/
/* Forward declarations                                                       */
/*============================================================================*/

/*============================================================================*/
/* Constants and macros                                                       */
/*============================================================================*/

#define MAX31856_F_INIT_VALID	0x01
#define MAX31856_READ_SIZE	6
#define MAX31856_READ_START	MAX31856_CJTH

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

extern SPI_HandleTypeDef hspi2;
/**
 * MAX31856 chip select port-pin structure
 */
typedef struct
{
	GPIO_TypeDef *CS_Port;	
	uint32_t CS_Pin;
	uint32_t flags;
}tMAX31856;

/**
 * Thermocouple sensor channel port-pin definition
 */
tMAX31856 tc[MAX_TC_CHANNELS] =
{
		{ GPIOB, TC_CH_0_Pin },
		{ GPIOB, TC_CH_1_Pin },
		{ GPIOB, TC_CH_2_Pin },
		{ GPIOB, TC_CH_3_Pin },

};

/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

uint8_t gMAX31856_buffer[MAX_TC_CHANNELS*MAX31856_READ_SIZE]; /**< MAX31856 Read Buffer */
uint8_t gExtCjEnabled=0; /**< External cold junction exist / not exist */
float extColdJOffset[MAX_TC_CHANNELS]; /**< External cold junction offset values */

/** Global data to read sample */
uint8_t tempData[MAX_TC_CHANNELS][MAX31856_READ_SIZE];
double tempSkip[MAX_TC_CHANNELS];

double filterData[MAX_TC_CHANNELS];
double filterCJData[MAX_TC_CHANNELS];

uint8_t extCJTemp[2];
float extTempf[2];
volatile uint32_t extCjErrCount = 0;

extern const tConfig_tc DefaultConfig_tc;

/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/

void max31856_select(tMAX31856 *tc)
{
	tc->CS_Port->BRR = tc->CS_Pin;
}

/*****************************************************************************/
void max31856_deselect(tMAX31856 *tc)
{
	tc->CS_Port->BSRR = tc->CS_Pin;
}

/*****************************************************************************/
/**
 * @brief MAX31856 write to register
 * @param ch		channel number
 * @param reg		register address offset
 * @param value		value to be written
 * @param length	length of transmitted data
 */
void max31856_wr_reg(tMAX31856 *ch, uint8_t reg, uint8_t *value, uint8_t length)
{
	uint8_t txData;
	uint8_t buffer[16];

	memcpy(buffer,value,length);

	txData = reg + MAX31856_WR_REG;

	max31856_select(ch);
	HAL_SPI_Transmit(&hspi2,&txData,1,1000);
	HAL_SPI_Transmit(&hspi2,buffer,length,1000);
	max31856_deselect(ch);
}

/*****************************************************************************/
/**
 * @brief MAX31856 read from register
 * @param ch		channel number
 * @param reg		register address offset
 * @param value		pointer tot value to be read
 * @param length	length of received data
 */
void max31856_rd_reg(tMAX31856 *ch, uint8_t reg, uint8_t *value, uint8_t length)
{
	uint8_t txData[16];
	uint8_t rxData[17];


	memset(rxData,0,16);
	memset(txData,0,16);

	txData[0] = reg;

	max31856_select(ch);
	//HAL_SPI_TransmitReceive(&hspi2,txData,rxData,length+1,1000);
	HAL_SPI_Transmit(&hspi2,txData,1,1000);
	HAL_SPI_Receive(&hspi2,rxData,length,1000);
	max31856_deselect(ch);

	memcpy(value,rxData,length);
}



/*
void max31856_dump_buffer(uint8_t *buffer,uint8_t len)
{
	int i=0;

	while(i < len)
	{
		uartLen = sprintf(uartBuffer,"%2.2X %2.2X %2.2X %2.2X\n\r",buffer[i+0],buffer[i+1],buffer[i+2],buffer[i+3]);
		HAL_UART_Transmit(&huart2, uartBuffer, uartLen,0x1000);
		i = i+4;
	}
	HAL_UART_Transmit(&huart2, "\n\r", 2,0x1000);
}
 */

/*****************************************************************************/
void max31856_read_raw(uint32_t ch, uint8_t *raw)
{
	max31856_rd_reg(&tc[ch],MAX31856_LTCBH,raw,4);
}

/*****************************************************************************/
void max31856_read_raw_buffered(uint32_t ch,uint8_t *raw)
{
	memcpy(raw,&gMAX31856_buffer[ch*MAX31856_READ_SIZE],MAX31856_READ_SIZE);
}

/*****************************************************************************/
void max31856_read_conf(uint32_t ch, uint8_t *data)
{
	max31856_rd_reg(&tc[ch],MAX31856_CR0,&data[0],2);
	max31856_rd_reg(&tc[ch],MAX31856_CJTO,&data[2],1);

	memcpy(&data[3], &extColdJOffset[ch], 4);

}

/*****************************************************************************/
void max31856_write_conf(uint32_t ch, uint8_t *data)
{
	memcpy(&extColdJOffset[ch], &data[3], 4);
	max31856_wr_reg(&tc[ch],MAX31856_CR0,&data[0],2);
	data[2] = extColdJOffset[ch] / 0.0625;
	max31856_wr_reg(&tc[ch],MAX31856_CJTO,&data[2],1);
}

/*****************************************************************************/
uint8_t max31856_read(uint32_t ch,float *value)
{
	uint8_t data[4];
	volatile int32_t raw = 0;

	if (!(tc[ch].flags & MAX31856_F_INIT_VALID))
	{
		*value = NAN;
		return 1;
	}

	max31856_rd_reg(&tc[ch],MAX31856_LTCBH,data,4);

	if (data[3] == 0) // Fault register
	{
		raw = ((uint32_t)data[0])<<24;
		raw |= ((uint32_t)data[1])<<16;
		raw |= ((uint32_t)data[2]<<8);
		raw = raw >> 13;

		*value = (float)raw * 0.0078125f;
	}
	else
	{
		*value = NAN;
	}
	return data[3];
}

/*****************************************************************************/
/**
 * @brief Read sample data from external cold junction STS31 thermal sensor
 * @param ch	Channel number
 * @param mux_ch	mux_ch or not
 */
void sampleExtCj(int ch, int mux_ch)
{
	static uint32_t extcjCounter=0;

	float extTemp;
	if (extcjCounter == 0)
	{

		if (EXTCJ_ReadTemperature(&extTemp) == 0)
		{
			if (fabs(extTempf[ch] - extTemp) > 0.3)
				extTempf[ch] = extTemp;
			else
				extTempf[ch] = extTempf[ch] * 0.99 +  extTemp *0.01;

			int16_t max31CJ = 0;

			max31CJ = (extTempf[ch]-extColdJOffset[ch]) / 0.00390625;


			extCJTemp[0] = (max31CJ & 0xFF00)>>8;
			extCJTemp[1] = (max31CJ & 0xFF);
		}
		else
		{
			extCjErrCount++;
		}
	}
	else if (extcjCounter > 250)
	{
		extcjCounter=0;
	}
	else
	{
		extcjCounter++;
	}
	//dbg_printf("ch : %d - Ext CJ : %f\r\n", ch, extTemp);

}

/*****************************************************************************/
/**
 * @brief Read Thermocouple sensor value. Function is generated for non-muxed thermocouple
 * boards.
 */
void max31856_nonmuxed_sample()
{
	int32_t temp;
	double tempf = 0;
	static uint8_t ch;
	static uint8_t counter;

	if (counter % 4)
		counter ++;
	else
	{
		/** Read External Cold Junction value if it is enabled */
		if (gExtCjEnabled)
			sampleExtCj(ch, 0);

		/**
		 * Write external cold junction temperature to MAX31856 because thermocouple
		 * sensor chip is using CJ value to calculate temperature.
		 */
		if (gExtCjEnabled)
			max31856_wr_reg(&tc[ch],MAX31856_CJTH,&extCJTemp[0],2);

		/** Read value from MAX31856 */
		HAL_Delay(100);
		max31856_rd_reg(&tc[ch],MAX31856_READ_START,&tempData[ch][0],MAX31856_READ_SIZE);

		/*if (tempData[ch][5] == 0)
		{*/
			temp =  (int32_t)tempData[ch][2] << 16;
			temp |= (int32_t)tempData[ch][3] << 8;
			temp |= (int32_t)tempData[ch][4];

			if (temp & 0x00800000)
				temp |= 0xFF000000;

			temp = temp >> 5;

			tempf = (float) temp * 0.0078125f;

			if (fabs(tempf - tempSkip[ch]) > 25)
			{
				tempSkip[ch] = tempf;
			}
			/** Filter data for rapid changes */
			if (fabs(filterData[ch]-tempf) > 0.3f)
				filterData[ch] = tempf;
			else
				filterData[ch] = filterData[ch]*0.998f + ((double)tempf)*0.002f;


			temp =  (filterData[ch] / 0.0078125f);
			temp = temp << 5;
			tempData[ch][2] = (temp & 0x00FF0000) >> 16;
			tempData[ch][3] = (temp & 0x0000FF00) >> 8;
			tempData[ch][4] = (temp & 0x000000FF);
		//}

#ifdef RTDEBUG
		dbg_printf("ch[%d]: %f -- %x \n",ch, tempf, tempData[ch][5]);
#endif
		/** Copy sensor value to global buffer */
		memcpy(&gMAX31856_buffer[ch*MAX31856_READ_SIZE],&tempData[ch],MAX31856_READ_SIZE);

		if(ch >= 3)
		{
			ch=0;
		}
		else
		{
			ch++;
		}

		counter = 0;
	}

}

/*****************************************************************************/
/**
 * @brief Initialize MAX31856 chip with cr0 c1 configuration
 * @param ch	Channel number
 * @param cr0	cr0 configuration
 * @param cr1	cr1 configuration
 * @return	0 or 1
 */
int max31856_init(tMAX31856 *ch,uint8_t cr0,uint8_t cr1)
{
	uint8_t datar[2] = {0};

	for (int i=0;i<10;i++)
	{
		max31856_wr_reg(ch,MAX31856_CR0,&cr0,1);
		max31856_wr_reg(ch,MAX31856_CR1,&cr1,1);

		max31856_rd_reg(ch,MAX31856_CR0,&datar[0],1);
		max31856_rd_reg(ch,MAX31856_CR1,&datar[1],1);
		ch->flags |= MAX31856_F_INIT_VALID;
		if ((cr0 == datar[0]) && (cr1 == datar[1]))
		{
			ch->flags |= MAX31856_F_INIT_VALID;
			return 0;
		}
		HAL_Delay(10);
	}
	return 1;
}

/*****************************************************************************/
void tc_init(int extcj)
{
	dbg_printf("Init TCs\n");
	if (extcj < 0)
	{
		dbg_printf("ExtCj Init Internal Err\n");
	}
	else if (extcj == 1)
	{
		/** Disable internal CJ sensing */
		gExtCjEnabled = 1; // this is a external CJ enabled board

		/** Not a muxed one but has external CJ Sensors */
		for (int i=0;i<MAX_TC_CHANNELS;i++)
		{
			extColdJOffset[i] = gBoardConfig.gConfig_tc.ch_conf[i].cjOffset.cjOff;
		}

	}

	dbg_printf("cj_offset 0: %d", gBoardConfig.gConfig_tc.ch_conf[0].cj_offset);
	dbg_printf("cj_offset 1: %d", gBoardConfig.gConfig_tc.ch_conf[1].cj_offset);
	dbg_printf("cj_offset 2: %d", gBoardConfig.gConfig_tc.ch_conf[2].cj_offset);
	dbg_printf("cj_offset 3: %d", gBoardConfig.gConfig_tc.ch_conf[3].cj_offset);
	dbg_printf("cr0 : %x", gBoardConfig.gConfig_tc.ch_conf[0].cr0);
	dbg_printf("cr1 : %x\r\n", gBoardConfig.gConfig_tc.ch_conf[0].cr1);


	uint32_t try = 0;

	for (int i=0;i<MAX_TC_CHANNELS;i++)
	{
		max31856_deselect(&tc[i]);
		HAL_Delay(10);

		try = 0;
		while(try < 20)
		{
			if (!max31856_init(&tc[i],gBoardConfig.gConfig_tc.ch_conf[i].cr0,gBoardConfig.gConfig_tc.ch_conf[i].cr1))
			{
				if (!gExtCjEnabled)
					max31856_wr_reg(&tc[i],MAX31856_CJTO,(unsigned char* )&gBoardConfig.gConfig_tc.ch_conf[i].cj_offset,1);
				dbg_printf("TC Ch %d OK\n",i+1);
				break;
			}
			else
			{
				dbg_printf("TC Ch %d Fail\n",i+1);
			}
			HAL_Delay(10);
			try++;
		}
	}

}


/*****************************************************************************/
void max31856_process()
{
	for (int i=0; i<MAX_TC_CHANNELS; i++)
		max31856_nonmuxed_sample();
}

/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2019,  All Rights Reserved. Confidential.      */
/******************************************************************************/
