/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2020 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        bus_comm.c
 * @brief       Bus Communication with ESP32 implementation.
 *
 * @author      Mehmet Fatih �zay
 * @date        2020-04-24
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

#define MAX_SPI_DATA 	512

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

uint32_t iapMailbox[2];
uint8_t enterBL = 0;
extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern uint8_t spiRxBuffer[MAX_SPI_DATA];
extern uint8_t espMsgRcvd;
uint8_t spiTxBuffer[MAX_SPI_DATA];    /**< SPI TX buffer */

/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/

uint32_t bus_cmd_both_read_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	uint8_t *data = &txData[0];
	*txLen = stmp3x_read_raw_buffered(data);
	data += 350;

	for (int i=0;i<MAX_TC_CHANNELS;i++)
	{
		max31856_read_raw_buffered(i,data);
		data+=TC_CH_SAMPLE_SIZE;
	}

	*txLen += TC_CH_SAMPLE_SIZE * MAX_TC_CHANNELS;

	return 0;
}

/******************************************************************************/
/**
 * @brief TC read sample from STM32 to ESP32 over SPI
 * @param rxData received msg
 * @param rxLen received msg length
 * @param txData send msg
 * @param txLen send msg length
 * @return uint32_t 0
 */
uint32_t bus_cmd_tc_read_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	uint8_t *data = &txData[0];
	for (int i=0;i<MAX_TC_CHANNELS;i++)
	{
		max31856_read_raw_buffered(i,data);
		data+=TC_CH_SAMPLE_SIZE;
	}

	*txLen = TC_CH_SAMPLE_SIZE * MAX_TC_CHANNELS;

	return 0;
}

/******************************************************************************/

/**
 * @brief PA read sample from STM32 to ESP32 over SPI
 * @param rxData received msg
 * @param rxLen received msg length
 * @param txData send msg
 * @param txLen send msg length
 * @return uint32_t 
 */
uint32_t bus_cmd_pa_read_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	uint8_t *data = &txData[0];
	*txLen = stmp3x_read_raw_buffered(data);
	return 0;
}

/******************************************************************************/

uint32_t bus_cmd_ping_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	return 0;
}

/******************************************************************************/

uint32_t bus_cmd_tc_conf_read_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	return 0;
}

/******************************************************************************/

uint32_t bus_cmd_tc_conf_write_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	return 0;
}

/******************************************************************************/

uint32_t bus_cmd_pa_conf_read_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	return 0;
}

/******************************************************************************/

uint32_t bus_cmd_pa_conf_write_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	return 0;
}

/******************************************************************************/

uint32_t bus_cmd_read_info_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	return 0;
}

/******************************************************************************/

/**
 * @brief Make STM32 enter bootloader
 * @param rxData received msg
 * @param rxLen received msg length
 * @param txData send msg
 * @param txLen send msg length
 * @return uint32_t 0
 */
uint32_t rt_bus_cmd_enter_bootloader (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	//bus_entering_bootlaoder_hook();
	enterBL = 1;

	return 0;
}

/******************************************************************************/

/**
 * @brief Reset STM32
 * @param rxData received msg
 * @param rxLen received msg length
 * @param txData send msg
 * @param txLen send msg length
 * @return uint32_t 
 */
uint32_t rt_bus_cmd_reset (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	iapMailbox[0] = 0;
	iapMailbox[1] = 0;
	//bus_reseting_hook();
	NVIC_SystemReset();
	return 0;
}

/******************************************************************************/

/**
 * @brief Bootloader dont jump to application 
 * @param rxData received msg
 * @param rxLen received msg length
 * @param txData send msg
 * @param txLen send msg length
 * @return uint32_t 
 */
uint32_t rt_bus_cmd_bl_stay (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	*txData = 0;
	txData[1] = 0;
	*txLen = 2;
	return 0;
}


/******************************************************************************/

uint32_t rt_bus_cmd_bl_read_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	return 0;
}

/******************************************************************************/
uint32_t rt_bus_cmd_bl_write_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{

	return 0;
}

/******************************************************************************/

uint32_t rt_bus_cmd_bl_erase_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{

	return 0;
}

/******************************************************************************/

/** Command Types */
const tBusCommand commands[] =
{
		{CMD_BOTH_Read,			&bus_cmd_both_read_handler},
		{CMD_TC_Read,			&bus_cmd_tc_read_handler},
		{CMD_PA_Read,			&bus_cmd_pa_read_handler},
		{CMD_PING, 				&bus_cmd_ping_handler},
		{CMD_TC_CONF_Read,		&bus_cmd_tc_conf_read_handler},
		{CMD_TC_CONF_Write,		&bus_cmd_tc_conf_write_handler},
		{CMD_PA_CONF_Read,		&bus_cmd_pa_conf_read_handler},
		{CMD_PA_CONF_Write,		&bus_cmd_pa_conf_write_handler},
		{CMD_Info_Read,			&bus_cmd_read_info_handler},
		{CMD_EnterBootloader, 	&rt_bus_cmd_enter_bootloader},
		{CMD_RESET,				&rt_bus_cmd_reset},
		{CMD_BL_Stay, 			&rt_bus_cmd_bl_stay},
		{CMD_BL_Read,			&rt_bus_cmd_bl_read_handler},
		{CMD_BL_Write,			&rt_bus_cmd_bl_write_handler},
		{CMD_BL_Erase,			&rt_bus_cmd_bl_erase_handler},
};

const int gCommandCount = (sizeof(commands)/sizeof(tBusCommand));

uint16_t const crc_ccitt_table[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/******************************************************************************/

static inline unsigned short crc_ccitt_byte(uint16_t crc, const uint8_t c)
{
	return (crc >> 8) ^ crc_ccitt_table[(crc ^ c) & 0xff];
}

/******************************************************************************/

unsigned short crc16(unsigned char *data_p, unsigned short length)
{
	unsigned int crc = 0x0000;
	while (length--)
		crc = crc_ccitt_byte(crc, *data_p++);
	return (crc>>8) | (crc<<8);;
}


/******************************************************************************/

/**
 * @brief SPI send/receive data format. Received from VTS bus communication
 * @param cmd message command type
 * @param datalength message length
 */
void bus_frame_pack(uint8_t cmd, uint16_t *datalength)
{
	uint16_t cCRC = 0;

	spiTxBuffer[0] = PRT_STX;

	//txBuffer[1] = (uBusId & 0xFF00)>>8;
	//txBuffer[2] = (uBusId & 0x00FF);

	spiTxBuffer[3] = (((*datalength)+1) & 0xFF00)>>8;
	spiTxBuffer[4] = (((*datalength)+1) & 0xFF);

	spiTxBuffer[5] = cmd;

	cCRC =  crc16(&spiTxBuffer[PRT_AD_MSB_IDX], MAX_SPI_DATA - 4);

	spiTxBuffer[MAX_SPI_DATA - 3] = (cCRC & 0xFF00)>>8;
	spiTxBuffer[MAX_SPI_DATA - 2] = (cCRC & 0x00FF);
	spiTxBuffer[MAX_SPI_DATA - 1] = PRT_ETX;
}

#if 0
/******************************************************************************/

void bus_transmit(uint16_t length)
{


	uint32_t count = 0;

	/*while ((&hspi1.State != HAL_SPI_STATE_READY) & (count < 65000))
	{
		count ++;
	}*/

	//HAL_SPI_Transmit_DMA(&hspi1, txBuffer, 512);
	HAL_SPI_Transmit(&hspi1, txBuffer, 512, 1000);
}

#endif
/******************************************************************************/



void bus_process(void)
{
	if (espMsgRcvd)
	{
		uint16_t calc_CRC = 0;
		uint16_t txSize = 0;

		memset(spiTxBuffer, 0, MAX_SPI_DATA);
		dbg_printf("[0] : %d - [1] : %d - cmd[5] : %d - [511] : %d\r\n", spiRxBuffer[0], spiRxBuffer[1], spiRxBuffer[5], spiRxBuffer[511]);
		spiTxBuffer[511] = 0x3;

		if ((spiRxBuffer[PRT_STX_IDX] == PRT_STX) && spiRxBuffer[MAX_SPI_DATA - PRT_ETX_LEFT_IDX] == PRT_ETX)
		{
			calc_CRC = crc16(&spiRxBuffer[PRT_AD_MSB_IDX], MAX_SPI_DATA-4);
			if (memcmp(&spiRxBuffer[MAX_SPI_DATA-3],&calc_CRC,2)==0)
			{
				for (int i=0;i<gCommandCount ;i++)
				{
					if (commands[i].cmd == spiRxBuffer[PRT_CMD_IDX])
					{
						uint8_t ret = commands[i].handler(&spiRxBuffer[PRT_DATA_S_IDX], MAX_SPI_DATA, &spiTxBuffer[PRT_DATA_S_IDX+1], &txSize);

						spiTxBuffer[PRT_DATA_S_IDX] = PRT_ACK;
						txSize ++;

						bus_frame_pack(commands[i].cmd,&txSize);
						//bus_transmit(MAX_SPI_DATA);
						break;
					}
				}
			}
			memset(spiRxBuffer, 0, MAX_SPI_DATA);

		}



		/* Disable the peripheral */
		hdma_spi1_tx.Instance->CCR &= ~DMA_CCR_EN;
		hdma_spi1_tx.DmaBaseAddress->IFCR  = (DMA_FLAG_GL1 << hdma_spi1_tx.ChannelIndex);
		/* Configure DMA Channel data length */
		hdma_spi1_tx.Instance->CNDTR = MAX_SPI_DATA;
		/* Configure DMA Channel destination address */
		hdma_spi1_tx.Instance->CPAR = (uint32_t)&hspi1.Instance->DR;
		/* Configure DMA Channel source address */
		hdma_spi1_tx.Instance->CMAR = (uint32_t)&spiTxBuffer[0];
		/* Enable the Peripheral */
		hdma_spi1_tx.Instance->CCR |= DMA_CCR_EN;


		espMsgRcvd = 0;


	}


}

/******************************************************************************/

void bus_comm_process(void)
{
	//bus_process();

	/*dbg_printf("main  :[0]: %x - [1] : %x - [2] : %x [3] : %x\n", spiRxBuffer[0], spiRxBuffer[1], spiRxBuffer[2], spiRxBuffer[3]);
	dbg_printf("main  :[0]: %x - [1] : %x - [2] : %x [3] : %x\n", spiRxBuffer[4], spiRxBuffer[5], spiRxBuffer[6], spiRxBuffer[7]);
	dbg_printf("rxBuffer  :[511]: %x \n", spiRxBuffer[SPI_RX_BUFFER_SIZE-1]);*/

	if (enterBL)
	{
		HAL_Delay(200);
		while(1)
		{
			//bl_jump();
		}
	}

}


/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2020,  All Rights Reserved. Confidential.      */
/******************************************************************************/
