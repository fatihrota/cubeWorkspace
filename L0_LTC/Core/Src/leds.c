/*
 * leds.c
 *
 *  Created on: Sep 5, 2017
 *      Author: LeventK
 */
#include "main.h"
#include "rt_config.h"
#include "leds.h"

void leds_init()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = LED_0_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_0_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LED_1_PIN;
	HAL_GPIO_Init(LED_1_PORT, &GPIO_InitStruct);


	leds_0_on();
	leds_1_on();
}


int32_t gui_led0_flash_delay=-1;
int32_t gui_led1_flash_delay=-1;

int32_t gui_led0_flash_off_delay = -1;
int32_t gui_led1_flash_off_delay = -1;

int32_t gui_led_ident = 0;
int32_t gui_led_ident_timer = 0;

void leds_0_flash(uint32_t delay)
{
	if (gui_led0_flash_delay != -1)
		leds_0_off();
	gui_led0_flash_delay = delay;
	leds_0_on();
}

void leds_1_flash(uint32_t delay)
{
	if (gui_led1_flash_delay != -1)
		return;
	gui_led1_flash_delay = delay;
	leds_1_off();
}

void leds_ident(int on)
{
	gui_led_ident = on;
	if (!on)
	{
		leds_1_on();
		gui_led_ident_timer = 0;
	}
}

void leds_counters()
{
	if (gui_led0_flash_delay > 0)
		gui_led0_flash_delay--;
	else if (gui_led0_flash_delay == 0)
	{
		gui_led0_flash_delay = -1;
		leds_0_off();
	}

	if (gui_led1_flash_delay > 0)
		gui_led1_flash_delay--;
	else if (gui_led1_flash_delay == 0)
	{
		gui_led1_flash_delay = -1;
		leds_1_on();
	}

	if (gui_led_ident)
	{
		gui_led_ident_timer++;
		if (gui_led_ident_timer == 100)
		{
			leds_1_off();
		}
		else if (gui_led_ident_timer == 200)
		{
			leds_1_on();
			gui_led_ident_timer = 0;
		}

	}
}
