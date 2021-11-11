/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2020 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        iap.c
 * @brief       Inter application implementation.
 *
 * @author		LeventK
 * @date		2017-09-26
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

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

typedef void (*pFunction)(void);

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/

void iap_PrepareForJump()
{
	//Disable all enabled interrupts
	HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Channel4_5_IRQn);
	HAL_NVIC_DisableIRQ(SysTick_IRQn);
	HAL_NVIC_DisableIRQ(USART1_IRQn);
	HAL_NVIC_DisableIRQ(SPI1_IRQn);

	HAL_DMA_Abort((&huart1)->hdmatx);
	HAL_DMA_Abort((&hspi1)->hdmatx);

	__disable_irq();

}

/******************************************************************************/
void iap_JumpToApplication()
{
	uint32_t  applicationStack;
	pFunction applicationEntry;

	iap_PrepareForJump();

	//Retrieve values
	applicationStack = *(__IO uint32_t *) (APPLICATION_ADDRESS);
	applicationEntry = (pFunction) *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);

	/*Set a valid stack pointer for the application */
	__set_MSP(applicationStack);


	/*Start the application */
	applicationEntry();
}

/******************************************************************************/
int iap_CheckApplication(void)
{
	SHA1_CTX sha1_ctx;
	uint8_t csha1[20];

	uint32_t length;
	unsigned char *p = (uint8_t *)(APPLICATION_ADDRESS);
	uint32_t JumpAddress= (__IO uint32_t*) (APPLICATION_ADDRESS + 4);


	if ((JumpAddress < APPLICATION_ADDRESS)|| (JumpAddress > APPLICATION_ADDRESS + MAX_APPLICATION_SIZE))
		return 1;

	length = *(__IO uint32_t*) (APPLICATION_LEN_ADDRESS);

	dbg_printf("Jump Addr : %d - %d - %d\r\n", length, (uint32_t *)length, &length);
	if ((length <= VECTOR_TABLE_SIZE) || (length > MAX_APPLICATION_SIZE))
	{
		return 2;
	}

	sha1_init(&sha1_ctx);
	sha1_update(&sha1_ctx,p,length);
	sha1_final(&sha1_ctx,csha1);

	if (memcmp(csha1,&p[length],20)!=0)
	{
		return 3;
	}
	return 0;
}

/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2020,  All Rights Reserved. Confidential.      */
/******************************************************************************/
