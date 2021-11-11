/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2020 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        bl_interface.c
 * @brief       Bootloader interface implementation.
 *
 * @author		LeventK
 * @date		2017-11-06
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

extern uint32_t __APPLICATION_ISR_START;
extern uint32_t __APPLICATION_ISR_END;
extern uint32_t VectorTable[64];
extern uint32_t iapMailbox[2];

typedef void (*pFunction)(void);

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern DMA_HandleTypeDef hdma_spi1_rx;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;

/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/

void bl_remapMemToSRAM( void )
{
	uint32_t size = 0;
	int32_t vecIndex = 0;
	static uint32_t  *app_start = &__APPLICATION_ISR_START;

	size = ((&__APPLICATION_ISR_END) - (&__APPLICATION_ISR_START));

	/** Remap Verctor Table and interrupts */
	__disable_irq();
	for(vecIndex = 0; vecIndex < size; vecIndex++){
		VectorTable[vecIndex] = app_start[vecIndex];
	}
	__HAL_SYSCFG_REMAPMEMORY_SRAM();
	__enable_irq();
}

/******************************************************************************/
void bl_prepare_jump()
{
	/** Disable interrupts to jump from application to bootloader */
	HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Channel4_5_IRQn);
	HAL_NVIC_DisableIRQ(SysTick_IRQn);
	HAL_NVIC_DisableIRQ(USART1_IRQn);
	HAL_NVIC_DisableIRQ(SPI1_IRQn);
	HAL_NVIC_DisableIRQ(SPI2_IRQn);

	/** Abort DMAs */
	HAL_DMA_Abort((&huart1)->hdmatx);
	HAL_DMA_Abort((&huart2)->hdmarx);
	HAL_DMA_Abort((&hspi1)->hdmatx);
	HAL_DMA_Abort((&hspi1)->hdmarx);

	__disable_irq();
	__HAL_SYSCFG_REMAPMEMORY_FLASH();
}

/******************************************************************************/
void bl_jump()
{
	bl_prepare_jump();

	uint32_t bootloaderStack;
	pFunction bootloaderEntry;

	/** Take bootloader address to jump */
	bootloaderStack = *(__IO uint32_t *) (BL_ADDRESS);
	bootloaderEntry = (pFunction) *(__IO uint32_t*) (BL_ADDRESS + 4);

	/** Write magin numbers to the box to let bootloader know that it is
	 * jumped from application */
	iapMailbox[0] = 0x524F5441;
	iapMailbox[1] = 0x54454b4e;

	/** Set a valid stack pointer for the bootloader */
	__set_MSP(bootloaderStack);

	/** Start the bootloader */
	bootloaderEntry();
}


/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2020,  All Rights Reserved. Confidential.      */
/******************************************************************************/
