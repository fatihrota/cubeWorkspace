/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2019 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        stpm3x.c
 * @brief       Power Analyzer implementation.
 *
 *
 * @author		Mehmet Fatih �zay
 * @date		2019-28-08
 *
 * @ingroup     vts
 * @{
 *****************************************************************************/

/*============================================================================*/
/* Includes                                                                   */
/*============================================================================*/

#include <stpm3x.h>
#include <string.h>
#include "main.h"
#include "debug.h"

/*============================================================================*/
/* Forward declarations                                                       */
/*============================================================================*/

/*============================================================================*/
/* Constants and macros                                                       */
/*============================================================================*/

#define STPM3x_RX_BUFFER_SIZE		360
#define STPM3x_TX_BUFFER_SIZE		360
#define STPM3x_SAMPLE_SIZE			350

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;

/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

uint8_t sndCommand[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0x7B};		/**< Get next register value */
uint8_t initCommand[5] = {0x00, 0xFF, 0xFF, 0xFF, 0xF0};	/**< Return to register 0 */
uint8_t configCR1[5] = {0xFF, 0x00, 0xA0, 0x00, 0x09};		/**< Not used */
uint8_t configCR2[5] = {0xFF, 0x01, 0x1E, 0x04, 0x0C};		/**< Not used */
uint8_t configCR3[5] = {0xFF, 0x05, 0x60, 0x00, 0xE7};		/**< Config CR3 register to set the latch */
uint8_t configCR3_2[5] = {0xFF, 0x05, 0x80, 0x00, 0x31};		/**< Config CR3 register to set the latch */
uint8_t readCR3[5] = {0x04, 0xFF, 0xFF, 0xFF, 0x83};		/**< Read CR3 register */
uint8_t configReset[5] = {0xFF, 0x05, 0x70, 0x00, 0x8C};

uint8_t frameDelay[5] = {0xFF, 0x1B, 0x00, 0x00, 0xF5};
uint8_t configBaudRate[5] = {0xFF, 0x1A, 0x06, 0x83, 0x18};
uint8_t configCrc[5] = {0xFF, 0x18, 0x40, 0x07, 0x65};
uint8_t configTimeOut[5] = {0xFF, 0x19, 0x00, 0x00, 0x94};


HAL_StatusTypeDef errorCode;
unsigned char rxDMABuffer[STPM3x_RX_BUFFER_SIZE];
unsigned char stpm_rxBuffer[STPM3x_SAMPLE_SIZE];
unsigned char stpm_txBuffer[STPM3x_SAMPLE_SIZE];

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	dbg_printf("RX STPM3x ALL\r\n");
	memcpy(&stpm_rxBuffer[0],&rxDMABuffer[10],STPM3x_SAMPLE_SIZE);
}

int stmp3x_read_raw_buffered(uint8_t *data)
{
	memcpy(data, &stpm_rxBuffer[0], STPM3x_SAMPLE_SIZE);
	return STPM3x_SAMPLE_SIZE;
}


/*****************************************************************************/
void stpmReset(void)
{
	HAL_UART_Transmit(&huart2, &configReset[0], 5, 1000);
	HAL_Delay(1000);
	/*HAL_DMA_Abort(&hdma_usart2_rx);
	HAL_Delay(1000);
	NVIC_SystemReset();*/


}

/*****************************************************************************/
void stpm_init(void)
{

	for (int i=0;i<STPM3x_SAMPLE_SIZE;i++)
		stpm_rxBuffer[i] = 0;
	for (int i=0;i<STPM3x_SAMPLE_SIZE;i++)
		stpm_txBuffer[i] = 0;

	//MX_USART2_UART_Init();

	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);   // enable idle line interru

	stpmReset();


	/** Initial command sequence to activate STPM3x to read periodically. */
	HAL_UART_Transmit(&huart2, &readCR3[0], 5, 1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&huart2, &readCR3[0], 5, 1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&huart2, &sndCommand[0], 5, 1000);
	HAL_Delay(100);
	HAL_UART_Transmit(&huart2, &configCR3_2[0], 5, 1000);

	/** Start UART receive via DMA */
	HAL_UART_Receive_DMA(&huart2, rxDMABuffer, STPM3x_RX_BUFFER_SIZE);


}


/*****************************************************************************/
void stpm_Process(void)
{

	/** Set LATCH */
	errorCode = HAL_UART_Transmit(&huart2, &configCR3_2[0], 5, 1000);
	HAL_Delay(20);

	if (errorCode != HAL_OK)
	{
		dbg_printf("errcode 1 : %d\n", errorCode);
	}

	/** Start from register 0 */
	errorCode = HAL_UART_Transmit(&huart2, &initCommand[0], 5, 1000);
	HAL_Delay(20);

	if (errorCode != HAL_OK)
	{
		dbg_printf("errcode 2 : %d\n", errorCode);
	}

	/** Read all registers */
	for (int i = 0; i < 70; i++)
	{
		errorCode = HAL_UART_Transmit(&huart2, &sndCommand[0], 5, 1000);
		HAL_Delay(10);
	}

	if (errorCode != HAL_OK)
	{
		dbg_printf("errcode 3 : %d\n", errorCode);
	}

	HAL_Delay(20);

#ifdef RTDEBUG
	//dbg_printf("RX Buffer : \n");
	int j = 0;
	for (int i = 0; i < 70; i++)
	{
		j = i *5;
		dbg_printf("[%d] - %x:%x:%x:%x:%x \n", i, stpm_rxBuffer[j], stpm_rxBuffer[j+1], stpm_rxBuffer[j+2], stpm_rxBuffer[j+3], stpm_rxBuffer[j+4]);
	}
#endif

	//HAL_Delay(1000);

}

/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2019,  All Rights Reserved. Confidential.      */
/******************************************************************************/
