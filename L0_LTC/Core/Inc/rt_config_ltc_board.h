/*
 * rt_config_tc_board.h
 *
 *  Created on: Nov 9, 2017
 *      Author: LeventK
 */

#ifndef RT_CONFIG_TC_BOARD_H_
#define RT_CONFIG_TC_BOARD_H_


#include "stm32l0xx_hal.h"


#define BUS_CAN_PING					1
#define BUS_CAN_RD_INFO					1
#define BUS_CAN_RESET					1
#define BUS_CAN_IDENT					1

#ifndef RT_BOOTLOADER

	#define BUS_CAN_RD_CONF					1
	#define BUS_CAN_WR_CONF					1
	#define BUS_CAN_RD_SAMPLE				1
	#define BUS_CAN_WR_SAMPLE				0
	#define BUS_CAN_RD						0
	#define BUS_CAN_WR						0
	#define BUS_CAN_WR_CALIBRATION			0
	#define BUS_CAN_RD_CALIBRATION			0
	#define BUS_CAN_ENTER_BOOTLOADER		1
#else
	#define BUS_CAN_BOOTLOADER_WR			1
	#define BUS_CAN_BOOTLOADER_RD			0
	#define BUS_CAN_BOOTLOADER_RDINFO		1
	#define BUS_CAN_BOOTLOADER_ERASE		1
	#define BUS_CAN_BOOTLOADER_STAY			1
#endif

#define LTC_CS_PORT				GPIOB
#define LTC_CS_PIN  			GPIO_PIN_12

#define LTC_INT_PORT			GPIOB
#define LTC_INT_PIN				GPIO_PIN_11


#define BP_PRESENCE_PORT		GPIOC
#define BP_PRESENCE_PIN			GPIO_PIN_3
#define ADDR_5_PORT		GPIOC
#define ADDR_5_PIN		GPIO_PIN_2
#define ADDR_4_PORT		GPIOC
#define ADDR_4_PIN		GPIO_PIN_1
#define ADDR_3_PORT		GPIOC
#define ADDR_3_PIN		GPIO_PIN_0
#define ADDR_2_PORT		GPIOC
#define ADDR_2_PIN		GPIO_PIN_15
#define ADDR_1_PORT		GPIOC
#define ADDR_1_PIN		GPIO_PIN_14
#define ADDR_0_PORT		GPIOC
#define ADDR_0_PIN		GPIO_PIN_13

#define LED_0_PORT		GPIOA
#define LED_0_PIN		GPIO_PIN_0
#define LED_1_PORT		GPIOA
#define LED_1_PIN		GPIO_PIN_1

#define SAMPLING_TIMER_PRESCALER	47999	/**< 48 Mhz oscillator 1ms timer input clock for this prescaler */
#define SAMPLING_TIMER_ARR			49		/**< 100ms update interrupt for sampling interrupt */

#define	SPI1_CLKPOL		SPI_POLARITY_LOW
#define SPI1_CLKPHA		SPI_PHASE_1EDGE
#define SPI1_PRESCALER	SPI_BAUDRATEPRESCALER_4
#define SPI1_MSBFIRST 	SPI_FIRSTBIT_MSB

static const tGPIOInit gInitGPIOs[] =
{
		/* Outputs */
		{
				.port = LED_0_PORT,
				.pin = LED_0_PIN,
				.mode = GPIO_MODE_OUTPUT_PP,
				.pull = GPIO_NOPULL,
				.speed = GPIO_SPEED_FREQ_LOW,
				.initialState = GPIO_PIN_RESET,
		},
		{
				.port = LED_1_PORT,
				.pin = LED_1_PIN,
				.mode = GPIO_MODE_OUTPUT_PP,
				.pull = GPIO_NOPULL,
				.speed = GPIO_SPEED_FREQ_LOW,
				.initialState = GPIO_PIN_RESET,
		},
		{
				.port = LTC_CS_PORT,
				.pin = LTC_CS_PIN,
				.mode = GPIO_MODE_OUTPUT_PP,
				.pull = GPIO_NOPULL,
				.speed = GPIO_SPEED_FREQ_LOW,
				.initialState = GPIO_PIN_SET,
		},

		/* Inputs */
		{
				.port = BP_PRESENCE_PORT,
				.pin = BP_PRESENCE_PIN,
				.mode = GPIO_MODE_INPUT,
				.pull = GPIO_PULLUP,
				.speed = GPIO_SPEED_FREQ_LOW,
				.initialState = GPIO_PIN_RESET,
		},
		{
				.port = ADDR_5_PORT,
				.pin = ADDR_5_PIN,
				.mode = GPIO_MODE_INPUT,
				.pull = GPIO_PULLUP,
				.speed = GPIO_SPEED_FREQ_LOW,
				.initialState = GPIO_PIN_RESET,
		},
		{
				.port = ADDR_4_PORT,
				.pin = ADDR_4_PIN,
				.mode = GPIO_MODE_INPUT,
				.pull = GPIO_PULLUP,
				.speed = GPIO_SPEED_FREQ_LOW,
				.initialState = GPIO_PIN_RESET,
		},
		{
				.port = ADDR_3_PORT,
				.pin = ADDR_3_PIN,
				.mode = GPIO_MODE_INPUT,
				.pull = GPIO_PULLUP,
				.speed = GPIO_SPEED_FREQ_LOW,
				.initialState = GPIO_PIN_RESET,
		},
		{
				.port = ADDR_2_PORT,
				.pin = ADDR_2_PIN,
				.mode = GPIO_MODE_INPUT,
				.pull = GPIO_PULLUP,
				.speed = GPIO_SPEED_FREQ_LOW,
				.initialState = GPIO_PIN_RESET,
		},
		{
				.port = ADDR_1_PORT,
				.pin = ADDR_1_PIN,
				.mode = GPIO_MODE_INPUT,
				.pull = GPIO_PULLUP,
				.speed = GPIO_SPEED_FREQ_LOW,
				.initialState = GPIO_PIN_RESET,
		},
		{
				.port = ADDR_0_PORT,
				.pin = ADDR_0_PIN,
				.mode = GPIO_MODE_INPUT,
				.pull = GPIO_PULLUP,
				.speed = GPIO_SPEED_FREQ_LOW,
				.initialState = GPIO_PIN_RESET,
		},
		{
				.port = LTC_INT_PORT,
				.pin = LTC_INT_PIN,
				.mode = GPIO_MODE_IT_RISING,
				.pull = GPIO_NOPULL,
				.speed = GPIO_SPEED_FREQ_HIGH,
				.initialState = GPIO_PIN_RESET,
		},
		{
				.port = GPIOC,
				.pin = GPIO_PIN_5,
				.mode = GPIO_MODE_OUTPUT_PP,
				.pull = GPIO_PULLUP,
				.initialState = GPIO_PIN_SET,
		},
};


static const int gGPIOPinCount = (sizeof(gInitGPIOs)/sizeof(tGPIOInit));

#endif /* RT_CONFIG_TC_BOARD_H_ */
