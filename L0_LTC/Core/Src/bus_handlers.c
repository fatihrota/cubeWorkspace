/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2019 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        bus_handlers.c
 * @brief       Bus Handler implementation. It consists the hook function.
 * 				When bus_proto received command from master board, it calls the related
 * 				callback function. If command is board specific, Then defined hook
 * 				function is called.
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
#include "ltc2983.h"
#include "main.h"
#include "conf.h"
#include "debug.h"

/*============================================================================*/
/* Forward declarations                                                       */
/*============================================================================*/

/*============================================================================*/
/* Constants and macros                                                       */
/*============================================================================*/

#define CH_COUNT		MAX_TC_CHANNELS
#define CH_CONF_LENGTH	12

#define CH_SAMPLE_SIZE	12



/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

uint8_t pingReceived = 0;
uint32_t pingTime = 0xFFFF;

/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/

/**
 * Get Number of thermocouple channel
 * @return
 */
int bus_get_max_ch()
{
	return CH_COUNT;
}

/*****************************************************************************/

/**
 * Get Channel configuration data length
 * @return
 */
int bus_get_ch_conf_length()
{
	return CH_CONF_LENGTH;
}

/*****************************************************************************/
/**
 * Wrapper for reading sample from channel
 * @param ch	channel number
 * @param data	pointer to data
 * @return
 */
int bus_read_ch_sample(uint8_t ch, uint8_t* data)
{
	memcpy(data, (unsigned char*)&chData[ch], CH_SAMPLE_SIZE);
	return CH_SAMPLE_SIZE;
}

/*****************************************************************************/
/**
 * Wrapper for reading sample from all channels
 * @param data	pointer to data
 * @return
 */
int bus_read_all_sample(uint8_t *data)
{
	for (int i=0;i<CH_COUNT;i++)
	{
		data+=bus_read_ch_sample(i,data);
	}
	return CH_COUNT * CH_SAMPLE_SIZE;
}

/*****************************************************************************/
/**
 * Wrapper for reading configuration from channel
 * @param ch	channel number
 * @param data	pointer to data
 * @return
 */
int bus_read_ch_conf(uint8_t ch, uint8_t *data)
{
	memcpy(&data[0], (const char *)&(chConfig[ch].config), 4);
	memcpy(&data[4], &gConfig.ch_conf[ch].cjOffset.cjOffBytes[0], 4);
	data[8] = gConfig.ch_conf[ch].enabled;

	return CH_CONF_LENGTH;
}

/*****************************************************************************/
/**
 * Wrapper for reading configuration from all channels
 * @param data	pointer to data
 * @return
 */
int bus_read_all_conf(uint8_t *data)
{

	for (int i=0;i<CH_COUNT;i++)
	{
		data+=bus_read_ch_conf(i,data);
	}
	return (CH_COUNT*CH_CONF_LENGTH);
}

/*****************************************************************************/
/**
 * Wrapper for writing configuration to channel
 * @param ch	channel number
 * @param data	pointer to data
 * @return
 */
int bus_write_ch_conf(uint8_t ch,uint8_t *data)
{
	memcpy((const char *)&(chConfig[ch].config), &data[0], 4);
	memcpy(&gConfig.ch_conf[ch].cjOffset.cjOffBytes[0], &data[4], 4);

	gConfig.ch_conf[ch].enabled = data[8];
	gConfig.ch_conf[ch].sensorType = chConfig[ch].config;
	LTC_TC_configureChannel(ch, chConfig[ch].config, gConfig.ch_conf[ch].CJ_channel,
											gConfig.ch_conf[ch].singleEnded, gConfig.ch_conf[ch].openCktDetect, gConfig.ch_conf[ch].openCktDetectCurrent);

	dbg_printf("Ch : %d Write config : %d offset : %f\n", ch, chConfig[ch].config, gConfig.ch_conf[ch].cjOffset.cjOff);

	return CH_CONF_LENGTH;
}

/*****************************************************************************/
/**
 * Wrapper for writing configuration to all channels
 * @param data	pointer to data
 * @param length (unusued)
 * @return
 */
int bus_write_all_conf(uint8_t *data,uint32_t length)
{
	for (int i=0;i<CH_COUNT;i++)
	{
		data+=bus_write_ch_conf(i,data);
	}

	return CH_COUNT * CH_CONF_LENGTH;
}

/*****************************************************************************/
/**
 * bus save conf
 * @return
 */
int bus_save_conf()
{
	conf_save();
	return 0;
}

/*****************************************************************************/
/**
 * bus_entering_bootlaoder_hook
 */
void bus_entering_bootlaoder_hook()
{

}

/*****************************************************************************/
/**
 * bus_ping_received_hook
 */
void bus_ping_received_hook()
{
	pingReceived = 1;
	pingTime = HAL_GetTick();
}

/*****************************************************************************/
/**
 * bus_reseting_hook
 */
void bus_reseting_hook()
{

}

/*****************************************************************************/
/**
 * bus_rd_info_hook
 * @param data
 * @return
 */
int bus_rd_info_hook(uint8_t *data)
{

	return 0;
}

/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2019,  All Rights Reserved. Confidential.      */
/******************************************************************************/


