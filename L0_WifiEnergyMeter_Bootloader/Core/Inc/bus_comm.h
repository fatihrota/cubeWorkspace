/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2020 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        bus_comm.h
 * @brief       Bus Communication with ESP32 function prototypes.
 *
 *
 * @author		Mehmet Fatih �zay
 * @date		2020-04-24
 *
 * @ingroup     vts
 * @{
 *****************************************************************************/
/*============================================================================*/
/* Includes                                                                   */
/*============================================================================*/

#ifndef BUS_COMM_H_
#define BUS_COMM_H_

/*============================================================================*/
/* Forward declarations                                                       */
/*============================================================================*/

/*============================================================================*/
/* Constants and macros                                                       */
/*============================================================================*/

/** SPI Communication protocol which is taken from VTS bus communication */
//	0		1		2		3		4		5		6		n-3		n-2		n-1
//			MSB		LSB		MSB 	LSB						MSB		LSB
//	STX		AD0		AD1		LEN0	LEN1	CMD		DATA	CRC0	CRC1	ETX

#define PRT_STX_IDX			0
#define PRT_AD_MSB_IDX		1
#define PRT_AD_LSB_IDX		2
#define PRT_LEN_MSB_IDX		3
#define PRT_LEN_LSB_IDX		4
#define PRT_CMD_IDX			5
//#define PRT_CMD_STAT_IDX	6
#define PRT_DATA_S_IDX		6
#define PRT_CRC_MSB_LEFT_IDX		3
#define PRT_CRC_LSB_LEFT_IDX		2
#define PRT_ETX_LEFT_IDX			1

#define PRT_STX		0x01
#define PRT_ETX		0x03

#define PRT_ACK		0x00
#define PRT_NCK		0x01

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/** Command types between ESP32 and STM32 */
typedef enum
{
	CMD_PING=0x00,

	CMD_TC_CONF_Read,
	CMD_TC_CONF_Write,
	CMD_PA_CONF_Read,
	CMD_PA_CONF_Write,
	CMD_TC_Read,
	CMD_PA_Read,
	CMD_BOTH_Read,
	CMD_Read,
	CMD_Write,

	CMD_EnterBootloader = 0xC0,

	CMD_Info_Read 	= 0xD0,

	CMD_BL_Read 	= 0xFA,
	CMD_BL_Write 	= 0xFB,
	CMD_BL_ReadInfo = 0xFC,
	CMD_BL_Erase 	= 0xFD,
	CMD_BL_Stay 	= 0xFE,

	CMD_RESET 		= 0xFF,
}eBusCommand;


typedef uint32_t (*tCmdHandler)(uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen);

/**
 * @brief bus message handler to call required function according to command type
 * 
 */
typedef struct
{
	uint8_t 	cmd;		/**< Bus command type */
	tCmdHandler handler;	/**< Corresponding function */
}tBusCommand;

/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

/*============================================================================*/
/* Declarations                                                               */
/*============================================================================*/
/**
 * @brief Unused
 */
void spi_data_received(void);

/**
 * @brief Loop process
 */
void bus_comm_process(void);


#endif /* BUS_COMM_H_ */

/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2020,  All Rights Reserved. Confidential.      */
/******************************************************************************/

