/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2019 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        iap.h
 * @brief       Inter application processes header file.
 *
 * @author      LeventK
 * @date        2017-09-26
 *
 * @ingroup     vts
 * @{
 *****************************************************************************/

/*============================================================================*/
/* Includes                                                                   */
/*============================================================================*/

#ifndef IAP_H_
#define IAP_H_

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
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/

/**
 * @brief Jump application from bootloader
 */
void iap_JumpToApplication();

/**
 * @brief Check the version, key, size etc. of the application firmware
 * @return int 
 */
int iap_CheckApplication(void);


#endif /* IAP_H_ */
/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2019,  All Rights Reserved. Confidential.      */
/******************************************************************************/
