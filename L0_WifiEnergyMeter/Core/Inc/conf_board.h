/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2020 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        conf_board.h
 * @brief       Power Analyzer Board Configuration structure definitions.
 *
 * @author      Mehmet Fatih Özay
 * @date        2020-04-20
 *
 * @ingroup     vts
 * @{
 *****************************************************************************/

/*============================================================================*/
/* Includes                                                                   */
/*============================================================================*/

#ifndef CONF_BOARD_H_
#define CONF_BOARD_H_

#include <stdio.h>
#include <stdint.h>
#include "MAX31856.h"

/*============================================================================*/
/* Forward declarations                                                       */
/*============================================================================*/

/*============================================================================*/
/* Constants and macros                                                       */
/*============================================================================*/

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/


/**
 * Digital Input slave card configuration.
 */
typedef struct
{
	uint8_t value;	
	union {
		float voltOff;
		unsigned char voltBytes[4];
	} voltCal;
	union {
		float curOff;
		unsigned char curBytes[4];
	} curCal;
}tConfig_pa;

/**
 * Channel Configuration of the MAX31856 thermocouple chip.
 */
typedef struct
{
	uint8_t cr0;		/**< Register cr0 settings */
	uint8_t cr1;		/**< Register cr1 settings */
	int8_t cj_offset;	/**< Cold junction offset settings */
	union {
		float cjOff;
		uint8_t cjOffBytes[4];
	} cjOffset;
}tChConfig;

/**
 * Thermocouple slave card configuration.
 */
typedef struct
{
	tChConfig ch_conf[MAX_TC_CHANNELS];
}tConfig_tc;

typedef struct
{
	uint8_t magic[4];	/**< Magic number to check integrity of configuration */
	uint32_t vers;		/**< Configuration version */
	tConfig_tc gConfig_tc;
	tConfig_pa gConfig_pa;
}tBoardConfig;

tBoardConfig gBoardConfig;


/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

/*============================================================================*/
/* Declarations                                                               */
/*============================================================================*/


#endif /* CONF_BOARD_H_ */
/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2020,  All Rights Reserved. Confidential.      */
/******************************************************************************/
