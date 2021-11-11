/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2020 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        bl_interface.h
 * @brief       Bootloader interface header file.
 *
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

#ifndef BL_INTERFACE_H_
#define BL_INTERFACE_H_

/*============================================================================*/
/* Forward declarations                                                       */
/*============================================================================*/

/*============================================================================*/
/* Constants and macros                                                       */
/*============================================================================*/

typedef struct
{
	uint32_t length;
	uint32_t version;
}tAppInfo;

#define VECTOR_TABLE_SIZE		0xC0

#define MAX_APPLICATION_SIZE	0xB0000
#define APPLICATION_ADDRESS     (uint32_t)0x08005000      /* Start user code address: ADDR_FLASH_PAGE_8 */
#define BL_ADDRESS			(uint32_t)0x08000000	      /**< Bootloader fw address (Start address of the flash) */

#define APPLICATION_LEN_ADDRESS (uint32_t) (APPLICATION_ADDRESS + VECTOR_TABLE_SIZE)
#define APPLICATION_CRC_ADDRESS	(uint32_t) (APPLICATION_ADDRESS + 0x50)

#define FLASH_START_ADDRESS		0x08000000
#define FLASH_END_ADDRESS		0x08030000	//Max allowed range 256k

#define RAM_START_ADDRESS		0x20000000
#define RAM_END_ADDRESS			0x20020000

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

/*============================================================================*/
/* Declarations                                                               */
/*============================================================================*/


/**
 * @brief Jump from application to bootloader
 */
void bl_jump();

/**
 * @brief remap Memory after jump from bootloader to application
 */
void bl_remapMemToSRAM(void);



#endif /* BL_INTERFACE_H_ */
/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2020,  All Rights Reserved. Confidential.      */
/******************************************************************************/
