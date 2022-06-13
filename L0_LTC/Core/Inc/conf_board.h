/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2019 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        conf_board.h
 * @brief       Thermocouple Board Configuration structure definitions.
 *
 * @author      LeventK
 * @date        2017-11-09
 *
 * @ingroup     vts
 * @{
 *****************************************************************************/

/*============================================================================*/
/* Includes                                                                   */
/*============================================================================*/

#ifndef CONF_BOARD_H_
#define CONF_BOARD_H_

#include "main.h"
/*============================================================================*/
/* Forward declarations                                                       */
/*============================================================================*/

/*============================================================================*/
/* Constants and macros                                                       */
/*============================================================================*/

#define MAX_TC_CHANNELS	20

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/**
 * Channel Configuration of the LTC2983 thermocouple chip.
 */
typedef struct
{
	uint32_t sensorType;
	uint32_t CJ_channel;
	uint32_t singleEnded;
	uint32_t openCktDetect;
	uint32_t openCktDetectCurrent;
	union {
		float cjOff;
		uint8_t cjOffBytes[4];
	} cjOffset;
	uint8_t enabled;
	uint8_t align[3];
}LTC_ChConfig;

/**
 * Thermocouple slave card configuration.
 */
typedef struct
{
	uint8_t magic[4];	/**< Magic number to check integrity of configuration */
	uint32_t vers;		/**< Configuration version */
	LTC_ChConfig ch_conf[MAX_TC_CHANNELS];
}tConfig;


/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

/*============================================================================*/
/* Declarations                                                               */
/*============================================================================*/


#endif /* CONF_BOARD_H_ */
/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2019,  All Rights Reserved. Confidential.      */
/******************************************************************************/
