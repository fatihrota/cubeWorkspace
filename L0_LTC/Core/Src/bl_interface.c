/*
 * bl_interface.c
 *
 *  Created on: Nov 6, 2017
 *      Author: LeventK
 */

#include "bl_interface.h"
#include "stm32l0xx_hal.h"

#define BL_ADDRESS			(uint32_t)0x08000000

extern uint32_t __APPLICATION_ISR_START;
extern uint32_t __APPLICATION_ISR_END;
extern uint32_t VectorTable[64];

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


void bl_remapMemToSRAM( void )
{
	uint32_t size = 0;
	int32_t vecIndex = 0;
	static uint32_t  *app_start = &__APPLICATION_ISR_START;

	size = ((&__APPLICATION_ISR_END) - (&__APPLICATION_ISR_START));

	__disable_irq();
	for(vecIndex = 0; vecIndex < size; vecIndex++){
		VectorTable[vecIndex] = app_start[vecIndex];
	}
	__HAL_SYSCFG_REMAPMEMORY_SRAM();
	__enable_irq();
}

typedef void (*pFunction)(void);


void bl_prepare_jump()
{

#if 0
	HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Channel4_5_6_7_IRQn);

	HAL_NVIC_DisableIRQ(USART1_IRQn);
	HAL_NVIC_DisableIRQ(USART2_IRQn);

	HAL_DMA_Abort((&huart1)->hdmarx);
	HAL_DMA_Abort((&huart1)->hdmatx);
	HAL_DMA_Abort((&huart2)->hdmatx);
#endif
	HAL_NVIC_DisableIRQ(TIM7_IRQn);
	HAL_NVIC_DisableIRQ(SysTick_IRQn);

	__disable_irq();
	__HAL_SYSCFG_REMAPMEMORY_FLASH();

}

extern uint32_t iapMailbox[2];

void bl_jump()
{
	bl_prepare_jump();

	uint32_t bootloaderStack;
	pFunction bootloaderEntry;

	//Retrieve values
	bootloaderStack = *(__IO uint32_t *) (BL_ADDRESS);
	bootloaderEntry = (pFunction) *(__IO uint32_t*) (BL_ADDRESS + 4);

	// Write magics to mailbox
	iapMailbox[0] = 0x524F5441;
	iapMailbox[1] = 0x54454b4e;

	//Set a valid stack pointer for the bootloader
	__set_MSP(bootloaderStack);


	//Start the bootloader
	bootloaderEntry();
}
