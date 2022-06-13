/*
 * rt_config.h
 *
 *  Created on: Sep 5, 2017
 *      Author: LeventK
 */

#ifndef RT_CONFIG_H_
#define RT_CONFIG_H_

#include "stm32l0xx_hal.h"

#ifndef DEBUG
#define DEBUG
#endif

extern UART_HandleTypeDef huart1;

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t pin;
	uint32_t mode;
	uint32_t pull;
	uint32_t speed;
	GPIO_PinState initialState;
}tGPIOInit;


/*
#ifndef RTDEBUG
#define RTDEBUG
#endif
*/

#define DEBUG_UART	(&huart2) //&huart1
#define BUS_UART	(&huart1)


#define BL_ADDRESS			(uint32_t)0x08000000
#define PROTO_LED_FLASH_TIME	25

#define BUS_BAUDRATE		115200
#define DBG_BUADRATE		115200

#define BUS_UART_PINSWAP
//#define ENABLE_BOOTLOADER_WDT
#define BOOTLOADED_APPLICATON

#define BOARD_TYPE_TC		0x01
#define BOARD_TYPE_PA		0x02
#define BOARD_TYPE_AI		0x03
#define BOARD_TYPE_SER		0x04
#define BOARD_TYPE_DI		0x05
#define BOARD_TYPE_LTC		0x06


#include "rt_config_ltc_board.h"


#define BUS_RX_BUFFER_SIZE		1248
#define BUS_TX_BUFFER_SIZE		1248




#endif /* RT_CONFIG_H_ */
