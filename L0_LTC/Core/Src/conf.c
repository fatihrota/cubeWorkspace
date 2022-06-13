/*
 * conf.c
 *
 *  Created on: Nov 8, 2017
 *      Author: LeventK
 */

#include "conf.h"
#include "conf_board.h"
#include "flash_if.h"
#include <string.h>

extern uint32_t __CONFIG_START;
extern const tConfig DefaultConfig;

void conf_writeconf (tConfig * c)
{
	static tConfig  *conf = (tConfig *)&__CONFIG_START;
	for (int i=0;i<3;i++)
	{
		FLASH_If_Erase((uint32_t)&__CONFIG_START,FLASH_PAGE_SIZE); 								//Delete 1 page
		FLASH_If_Write((uint32_t)&__CONFIG_START,(uint32_t *)c,sizeof(tConfig)/sizeof(uint32_t));
		if (memcmp(conf,c,sizeof(tConfig))==0)
		{
			break;
		}
	}

}

void conf_checkandload()
{
	static tConfig  *conf = (tConfig *)&__CONFIG_START;

	if ((memcmp(&conf->magic,&DefaultConfig.magic,4)!= 0) || (conf->vers != DefaultConfig.vers))
	{
		conf_writeconf((tConfig *)&DefaultConfig);
	}
	memcpy(&gConfig,conf,sizeof(tConfig));
}

void conf_save()
{
	static tConfig  *conf = (tConfig *)&__CONFIG_START;

	memcpy(&gConfig.magic,&DefaultConfig.magic,4); 		//Ensure that magic and vers is the right value
	gConfig.vers = DefaultConfig.vers;

	/*if (memcmp(&gConfig,conf,sizeof(tConfig))==0)		// Flash configuration and ram configuration matches so don't do anything
		return;
*/

	conf_writeconf(&gConfig);
}

void conf_reload()
{
	static tConfig  *conf = (tConfig *)&__CONFIG_START;
	memcpy(&gConfig,conf,sizeof(tConfig));
}


void conf_init()
{
	FLASH_If_Init();
	conf_checkandload();
}
