/*
 * iap_common.h
 *
 *  Created on: Sep 28, 2017
 *      Author: LeventK
 */

#ifndef IAP_COMMON_H_
#define IAP_COMMON_H_

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

#endif /* IAP_COMMON_H_ */
