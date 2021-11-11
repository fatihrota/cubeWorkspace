/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2020 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        debug.c
 * @brief       Debugging implementation.
 *
 * @author		LeventK
 * @date		2017-09-05
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

#ifdef __GNUC__
	/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
	 set to 'Yes') calls __io_putchar() */
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
	#define GETCHAR_PROTOTYPE int __io_getchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
	#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

/* We need to implement own __FILE struct */
/* FILE struct is used from __FILE */
struct __FILE {
    int dummy;
};

/* You need this if you want use printf */
/* Struct FILE is implemented in stdio.h */
FILE __stdout;

/*============================================================================*/
/* Constants and macros                                                       */
/*============================================================================*/

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

unsigned char dbg_tx_buffer[DEBUG_TX_BUFFER_SIZE];
unsigned char dbg_tx_dma_buffer[DEBUG_TX_BUFFER_SIZE];

unsigned int dbg_tx_count = 0;

/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/

void debug_init()
{
	memset(dbg_tx_buffer,0,DEBUG_TX_BUFFER_SIZE);
	dbg_tx_count = 0;
}


/******************************************************************************/
void debug_transmit_buffer()
{
	uint32_t count = 0;
	while ((DEBUG_UART->gState != HAL_UART_STATE_READY) & (count < 65000))
	{
		//WDT_Feed();
		count ++;
	}
	memcpy(dbg_tx_dma_buffer,dbg_tx_buffer,dbg_tx_count);
	dbg_tx(dbg_tx_dma_buffer,dbg_tx_count);
	dbg_tx_count = 0;
}

/******************************************************************************/

PUTCHAR_PROTOTYPE
{
	if (ch != '\r')
	{
		dbg_tx_buffer[dbg_tx_count++] = ch;

		if ((ch == '\n') || (dbg_tx_count >= DEBUG_TX_BUFFER_SIZE))
		{
			dbg_tx_buffer[dbg_tx_count++] = '\r';
			debug_transmit_buffer();
		}
	}

	return ch;
}

/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2020,  All Rights Reserved. Confidential.      */
/******************************************************************************/
