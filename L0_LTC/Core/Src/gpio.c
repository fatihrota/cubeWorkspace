#include "stm32l0xx_hal.h"
#include "rt_config.h"

void GPIO_Configure(const tGPIOInit *conf, uint32_t count)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	for (int i=0;i<count;i++)
	{
		//if this pin is a output pin write initial stat for this pin befor configuring as an output.
		if (conf[i].mode == GPIO_MODE_OUTPUT_PP)
			HAL_GPIO_WritePin(conf[i].port,conf[i].pin,conf[i].initialState);

		GPIO_InitStruct.Mode = conf[i].mode;
		GPIO_InitStruct.Pin = conf[i].pin;
		GPIO_InitStruct.Pull = conf[i].pull;
		GPIO_InitStruct.Speed = conf[i].speed;

		HAL_GPIO_Init(conf[i].port,&GPIO_InitStruct);
	}
}
