/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2020 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        conf.c
 * @brief       Configuration implementation.
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
#include "conf.h"
#include "flash_if.h"

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

extern uint32_t __CONFIG_START;
extern const tBoardConfig DefaultConfig;

/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/


void conf_writeconf (tBoardConfig * c)
{
	static tBoardConfig  *conf = (tBoardConfig *)&__CONFIG_START;
	for (int i=0;i<3;i++)
	{
		FLASH_If_Erase((uint32_t)&__CONFIG_START,FLASH_PAGE_SIZE); 								//Delete 1 page
		FLASH_If_Write((uint32_t)&__CONFIG_START,(uint32_t *)c,sizeof(tBoardConfig)/sizeof(uint32_t));
		if (memcmp(conf,c,sizeof(tBoardConfig))==0)
		{
			dbg_printf("Write comp OK!!\r\n");
			break;
		}
		dbg_printf("Write comp not ok\r\n");
	}

}

/******************************************************************************/
void conf_checkandload()
{
	static tBoardConfig  *conf = (tBoardConfig *)&__CONFIG_START;

	if ((memcmp(&conf->magic,&DefaultConfig.magic,4)!= 0) || (conf->vers != DefaultConfig.vers))
	{
		dbg_printf("Load Default Conf\r\n");
		conf_writeconf((tBoardConfig *)&DefaultConfig);
	}
	dbg_printf("Load Check Conf\r\n");
	memcpy(&gBoardConfig,conf,sizeof(tBoardConfig));
}

/******************************************************************************/
void conf_save()
{
	static tBoardConfig  *conf = (tBoardConfig *)&__CONFIG_START;

	memcpy(&gBoardConfig.magic,&DefaultConfig.magic,4); 		//Ensure that magic and vers is the right value
	gBoardConfig.vers = DefaultConfig.vers;

	/*if (memcmp(&gConfig,conf,sizeof(tConfig))==0)		// Flash configuration and ram configuration matches so don't do anything
		return;
*/

	conf_writeconf(&gBoardConfig);
}

/******************************************************************************/
void conf_reload()
{
	static tBoardConfig  *conf = (tBoardConfig *)&__CONFIG_START;
	memcpy(&gBoardConfig,conf,sizeof(tBoardConfig));
}

/******************************************************************************/
void conf_init()
{
	FLASH_If_Init();
	conf_checkandload();
}

/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2020,  All Rights Reserved. Confidential.      */
/******************************************************************************/
