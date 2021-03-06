/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2019 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        conf_board.c
 * @brief       Board Configuration default values.
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

#include "conf_board.h"
#include "ltc2983.h"

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
tConfig gConfig;

const tConfig DefaultConfig =
{
	.magic = {'R','O','T','A'},
	.vers = 0x00000004,
	.ch_conf = {
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
		{SENSOR_TYPE__TYPE_T_THERMOCOUPLE,TC_COLD_JUNCTION_CH__12,TC_SINGLE_ENDED,TC_OPEN_CKT_DETECT__YES,TC_OPEN_CKT_DETECT_CURRENT__10UA,{0},1},
	},
};
/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/

/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2019,  All Rights Reserved. Confidential.      */
/******************************************************************************/
