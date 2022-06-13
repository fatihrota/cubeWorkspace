/*
 * rt_bus_proto.c
 *
 *  Created on: Sep 5, 2017
 *      Author: LeventK
 */

#include <string.h>
#include "rt_bus_proto.h"
#include "crc16.h"
#include "rt_config.h"
#include "leds.h"
#include "debug.h"

unsigned char rxDMABuffer[BUS_RX_BUFFER_SIZE];
unsigned char rxBuffer[BUS_RX_BUFFER_SIZE];
unsigned char txBuffer[BUS_TX_BUFFER_SIZE];

uint16_t uBusId = 0x0000;
uint32_t isFrameReady=0;
uint32_t rxFrameSize = 0;
uint32_t lastRxTime = 0;

extern uint8_t pingReceived;
extern uint32_t pingTime;

#ifndef RT_BOOTLOADER
//extern IWDG_HandleTypeDef hiwdg;
#endif


uint32_t gFrameCount = 0;

uint16_t rt_bus_get_bid()
{
	uint16_t ret=0;
	GPIO_InitTypeDef GPIO_InitStruct;

	//Configure GPIO pin : PtPin
	GPIO_InitStruct.Pin = ADDR_0_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADDR_0_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = ADDR_1_PIN;
	HAL_GPIO_Init(ADDR_1_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = ADDR_2_PIN;
	HAL_GPIO_Init(ADDR_2_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = ADDR_3_PIN;
	HAL_GPIO_Init(ADDR_3_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = ADDR_4_PIN;
	HAL_GPIO_Init(ADDR_4_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = ADDR_5_PIN;
	HAL_GPIO_Init(ADDR_5_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = BP_PRESENCE_PIN;
	HAL_GPIO_Init(BP_PRESENCE_PORT, &GPIO_InitStruct);


	while (HAL_GPIO_ReadPin(BP_PRESENCE_PORT,BP_PRESENCE_PIN)) // Backplane didn't recognized
	{
		leds_0_off();
		leds_1_on();
		HAL_Delay(500);
		leds_0_on();
		leds_1_off();
		HAL_Delay(500);
	}

	//HAL_Delay(500);

	if (!HAL_GPIO_ReadPin(ADDR_0_PORT,ADDR_0_PIN))
		ret |= (1<<0);
	if (!HAL_GPIO_ReadPin(ADDR_1_PORT,ADDR_1_PIN))
		ret |= (1<<1);
	if (!HAL_GPIO_ReadPin(ADDR_2_PORT,ADDR_2_PIN))
		ret |= (1<<2);
	if (!HAL_GPIO_ReadPin(ADDR_3_PORT,ADDR_3_PIN))
		ret |= (1<<3);
	if (!HAL_GPIO_ReadPin(ADDR_4_PORT,ADDR_4_PIN))
		ret |= (1<<4);
	if (!HAL_GPIO_ReadPin(ADDR_5_PORT,ADDR_5_PIN))
		ret |= (1<<5);



	return ret;
}
void rt_bus_init()
{
	__disable_irq();
	for (int i=0;i<BUS_RX_BUFFER_SIZE;i++)
		rxBuffer[i] = 0;
	for (int i=0;i<BUS_TX_BUFFER_SIZE;i++)
		txBuffer[i] = 0;
	__enable_irq();
	uBusId = rt_bus_get_bid();

	dbg_printf("Board Init BUS ID %4.4X\n",uBusId);


	// Enable DMA Mode for receive in Circular Buffer Mode which allows for continuous data reception without requiring to be retriggered

	HAL_DMA_Start(BUS_UART->hdmarx, (uint32_t)&BUS_UART->Instance->RDR, (uint32_t)rxDMABuffer, BUS_RX_BUFFER_SIZE);

	// Enable the DMA transfer for the receiver request by setting the DMAR bit in the UART CR3 register
	SET_BIT(BUS_UART->Instance->CR3, USART_CR3_DMAR);

	// Enable Idle Detection Interrupt
	SET_BIT(BUS_UART->Instance->CR1 ,USART_CR1_IDLEIE);
	/*while(1)
	{

		leds_0_off();
		dbg_printf("Received %u\n",(unsigned int)BUS_UART->hdmarx->Instance->CNDTR);
		HAL_Delay(500);
		leds_0_on();
		HAL_Delay(100);

	}*/
}

//  0 		1		2		3		4		5		6		n+6		n+7		n+8
//	0		1		2		3		4		5		6		n-3		n-2		n-1
//			MSB		LSB		MSB 	LSB						MSB		LSB
//	STX		AD0		AD1		LEN0	LEN1	CMD		DATA	CRC0	CRC1	ETX
//										CMD = Valid CMD means data ok
//										CMD = 0xFF means error followed by error code


void rt_bus_proto_frame_pack(uint8_t cmd, uint16_t *datalength)
{
	uint16_t cCRC = 0;


	txBuffer[0] = PRT_STX;

	txBuffer[1] = (uBusId & 0xFF00)>>8;
	txBuffer[2] = (uBusId & 0x00FF);

	txBuffer[3] = (((*datalength)+1) & 0xFF00)>>8;
	txBuffer[4] = (((*datalength)+1) & 0xFF);

	txBuffer[5] = cmd;

	cCRC =  crc16(&txBuffer[PRT_AD_MSB_IDX], *datalength+5);

	txBuffer[*datalength + 6] = (cCRC & 0xFF00)>>8;
	txBuffer[*datalength + 7] = (cCRC & 0x00FF);
	txBuffer[*datalength + 8] = PRT_ETX;

	*datalength += 9;

}

void rt_bus_proto_transmit(uint16_t length)
{


	uint32_t count = 0;
	while ((BUS_UART->gState != HAL_UART_STATE_READY) & (count < 65000))
	{
		count ++;
	}

	HAL_UART_Transmit_DMA(BUS_UART,txBuffer,length);

}

extern void rt_bus_cmd_process();

void rt_bus_proto_clear()
{
	if (rxFrameSize)
	{
		if (HAL_GetTick() > (lastRxTime + 10))
		{
			__disable_irq();
			rxFrameSize = 0;
			__enable_irq();
		}
	}
#if 0
#ifndef RT_BOOTLOADER
	if (pingReceived)
	{
		pingReceived = 0;
	}
	else
	{
		if (HAL_GetTick() > (pingTime + 120000))
		{
			bus_reseting_hook();
			NVIC_SystemReset();
		}
	}
#endif
#endif

	rt_bus_cmd_process();
}
void rt_bus_proto_process()
{

	uint16_t pSize = 0;
	uint16_t addr = 0;
	uint16_t cCRC = 0;
	uint16_t txSize = 0;
	uint32_t ret;
	if (isFrameReady)
	{

		addr = ((uint16_t)rxBuffer[PRT_AD_MSB_IDX] << 8) | (rxBuffer[PRT_AD_LSB_IDX]);

		if (addr == uBusId)
		{
			if ((rxBuffer[PRT_STX_IDX] == PRT_STX) && (rxBuffer[rxFrameSize-PRT_ETX_LEFT_IDX] == PRT_ETX)) // check stx and etx on received frame
			{
				pSize = ((uint16_t)rxBuffer[PRT_LEN_MSB_IDX] << 8) | (rxBuffer[PRT_LEN_LSB_IDX]);
				if (pSize == rxFrameSize - 8)
				{
					cCRC = crc16(&rxBuffer[PRT_AD_MSB_IDX], rxFrameSize-4);
					if (memcmp(&rxBuffer[rxFrameSize-3],&cCRC,2)==0)
					{
						for (int i=0;i<gCommandCount ;i++)
						{
							if (commands[i].cmd == rxBuffer[PRT_CMD_IDX])
							{
								leds_0_flash(PROTO_LED_FLASH_TIME);
								txSize = 0;
								gFrameCount++;

								ret = commands[i].handler(&rxBuffer[PRT_DATA_S_IDX],pSize-1,&txBuffer[PRT_DATA_S_IDX+1],&txSize);
								if (ret == RT_PROTO_OK)
								{
									txBuffer[PRT_DATA_S_IDX] = PRT_ACK;
									txSize ++;
								}
								else
								{
									if (ret != RT_PROTO_SkipReply)
									{
										txBuffer[PRT_DATA_S_IDX] = PRT_NCK;
										txBuffer[PRT_DATA_S_IDX+1] = ret;
										txSize = 2;
									}
								}
								if (ret != RT_PROTO_SkipReply)
								{
									rt_bus_proto_frame_pack(commands[i].cmd,&txSize);
									rt_bus_proto_transmit(txSize);
								}
								break;
							}
						}
					}
				}
			}
		}
		else
		{
			leds_1_on();
		}
		isFrameReady = 0;
		rxFrameSize = 0;
	}
	else
	{
		leds_0_on();
	}
}


extern void Board_SystickCallBack(void);

void HAL_SYSTICK_Callback(void)
{
	leds_counters();
	Board_SystickCallBack();
}

void rt_bus_proto_idle_dt(void)
{
	static uint16_t prevDMACnt=BUS_RX_BUFFER_SIZE;
	uint16_t currentDMACnt = BUS_UART->hdmarx->Instance->CNDTR;
	uint16_t size=0;
	uint16_t start = 0;

	lastRxTime = HAL_GetTick();

	if (BUS_UART->hdmarx->State == HAL_DMA_STATE_BUSY)
	{
		if (prevDMACnt > currentDMACnt) // we haven't looped back yet
		{
			size = prevDMACnt - currentDMACnt;

			if (size > BUS_RX_BUFFER_SIZE)
				return;

			start = (BUS_RX_BUFFER_SIZE - prevDMACnt);

			if (rxFrameSize + size < BUS_RX_BUFFER_SIZE)
			{
				memcpy(&rxBuffer[rxFrameSize],&rxDMABuffer[start],size);
				rxFrameSize += size;

				if (rxBuffer[rxFrameSize-1] == 0x03)
					isFrameReady = 0x01;
			}

			prevDMACnt = currentDMACnt;
		}
		else if (prevDMACnt < currentDMACnt)
		{
			size = prevDMACnt;
			if (size > BUS_RX_BUFFER_SIZE)
				return;
			start = (BUS_RX_BUFFER_SIZE - prevDMACnt);

			if (rxFrameSize + size < BUS_RX_BUFFER_SIZE)
			{
				memcpy(&rxBuffer[rxFrameSize],&rxDMABuffer[start],size);
				rxFrameSize +=size;
			}

			size = BUS_RX_BUFFER_SIZE - currentDMACnt;
			start = 0;
			if (rxFrameSize + size < BUS_RX_BUFFER_SIZE)
			{
				memcpy(&rxBuffer[rxFrameSize],&rxDMABuffer[start],size);
				rxFrameSize += size;

				if (rxBuffer[rxFrameSize-1] == 0x03)
					isFrameReady = 0x01;
			}
			prevDMACnt = currentDMACnt;
		}
		else
		{
			return;
		}
#ifndef RT_BOOTLOADER
		rt_bus_proto_process();
#endif
	}

}

