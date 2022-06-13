
#include "rt_bus_common_commands.h"
#include "rt_config.h"
#include "iap_common.h"
#include "leds.h"
#include <string.h>
#include <stdio.h>
#include "bl_interface.h"

#ifndef RT_BOOTLOADER
extern int bus_get_ch_conf_length();
extern int bus_get_max_ch();



int rt_bus_get_bit_count(uint8_t data,uint8_t chCount)
{
	int ret = 0;
	int mask = 0;

	for (int i=0;i<chCount;i++)	// check if unsupported channel is flagged
	{
		mask |= (1<<i);
	}

	if ((~mask) & data)	// if unsupported ch is flagged than return 0
		return 0;

	for (int i=0;i<chCount;i++)
	{
		if (data & (1<<i))
		{
			ret ++;
		}
	}
	return ret;
}
#endif

#if (BUS_CAN_PING == 1)
extern void bus_ping_received_hook();
uint32_t rt_bus_cmd_ping_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	if (rxLen != 1)
	{
		return RT_PROTO_FrameError;
	}

	bus_ping_received_hook();

	txData[0] = rxData[0] + 1;
	*txLen = 1;
	return RT_PROTO_OK;
}
#endif

#if (BUS_CAN_IDENT == 1)
uint32_t rt_bus_cmd_ident_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	if (rxLen != 1)
	{
		return RT_PROTO_FrameError;
	}


	leds_ident(rxData[0]);

	*txLen = 0;
	return RT_PROTO_OK;
}
#endif


#if (BUS_CAN_RESET == 1)
extern void bus_reseting_hook();
extern uint32_t iapMailbox[2];
uint32_t rt_bus_cmd_reset (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	iapMailbox[0] = 0;
	iapMailbox[1] = 0;
	bus_reseting_hook();
	NVIC_SystemReset();
	return RT_PROTO_OK;
}
#endif

#if (BUS_CAN_RD_CONF == 1)
extern int bus_read_ch_conf(uint8_t ch, uint8_t *data);
extern int bus_read_all_conf(uint8_t *data);
uint32_t rt_bus_cmd_conf_read_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	uint32_t length=0;
	int ret = 0;
	int chCount;

	if ((rxData[0] == 0)||(rxData[0] == 0xFF))
	{
		ret = bus_read_all_conf(&txData[0]);
		if (ret < 0)
			return RT_PROTO_DataError;
		length = ret;
	}
	else
	{
		chCount = bus_get_max_ch();
		if (rt_bus_get_bit_count(rxData[0],chCount) == 0)
			return RT_PROTO_DataError;

		for (int i=0;i<8;i++)
		{
			if ((rxData[0] & (1<<i)) == (1<<i))
			{
				ret = bus_read_ch_conf(i,&txData[length]);
				if (ret < 0)
					return RT_PROTO_DataError;
				length += ret;
			}
		}
	}
	*txLen = length;
	return RT_PROTO_OK;
}
#endif

#if (BUS_CAN_WR_CONF == 1)
extern int bus_write_ch_conf(uint8_t ch,uint8_t *data);
extern int bus_write_all_conf(uint8_t *data,uint32_t length);
extern int bus_save_conf();


uint32_t rt_bus_cmd_conf_write_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	uint32_t length=1;
	int ret;
	int chCount = 0;

	chCount = bus_get_max_ch();

	if ((rxData[0] == 0)||(rxData[0] == 0xFF))
	{
		if (bus_get_ch_conf_length() * chCount != rxLen-1)	// check received data length matches boards requested length
			return RT_PROTO_DataError;

		ret = bus_write_all_conf(&rxData[1],rxLen-1);

		if (ret < 0)
			return RT_PROTO_DataError;
	}
	else
	{
		if ((bus_get_ch_conf_length() * rt_bus_get_bit_count(rxData[0],chCount)) + 1  != rxLen)  // check received data length matches boards requested length
			return RT_PROTO_DataError;

		for (int i=0;i<chCount;i++)
		{
			if ((rxData[0] & (1<<i)) == (1<<i))
			{
				ret = bus_write_ch_conf(i,&rxData[length]);
				if (ret < 0)
					return RT_PROTO_DataError;
				length += ret;
			}
		}
	}

	ret = bus_save_conf();
	if (ret < 0)
		return RT_PROTO_DataError;

	*txLen = 0;
	return RT_PROTO_OK;

}
#endif


#if (BUS_CAN_RD_SAMPLE == 1)
extern int bus_read_ch_sample(uint8_t ch, uint8_t* data);
extern int bus_read_all_sample(uint8_t *data);

uint32_t rt_bus_cmd_smp_read_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	int length=0;
	int ret=0;
	int chCount;

	if ((rxData[0] == 0)||(rxData[0] == 0xFF))
	{
		ret = bus_read_all_sample(&txData[0]);
		if (ret < 0)
			return RT_PROTO_DataError;
		length = ret;
	}
	else
	{
		chCount = bus_get_max_ch();
		if (rt_bus_get_bit_count(rxData[0],chCount) == 0)
			return RT_PROTO_DataError;


		for (int i=0;i<8;i++)
		{
			if ((rxData[0] & (1<<i)) == (1<<i))
			{
				ret = bus_read_ch_sample(i,&txData[length]);
				if (ret < 0)
					return RT_PROTO_DataError;
				length += ret;
			}
		}
	}
	*txLen = length;
	return RT_PROTO_OK;
}
#endif

#if (BUS_CAN_WR_SAMPLE == 1)
uint32_t rt_bus_cmd_smp_write_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	return RT_PROTO_OK;
}

#endif

#if ((BUS_CAN_RD_INFO == 1) || (BUS_CAN_BOOTLOADER_RDINFO == 1))
extern int bus_rd_info_hook(uint8_t *data);
extern const tAppInfo appinfo;
extern const uint8_t appsha[20];
extern uint32_t gFrameCount;

uint32_t rt_bus_cmd_read_info_handler(uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	uint32_t length = 0;

	for (int i=11;i>=0;i--)
	{
		txData[length++] = *((uint8_t *)UID_BASE + i);
	}


	sprintf((char *)&txData[length],"%s %s",__DATE__,__TIME__);
	length += 32;

	memcpy(&txData[length],&appinfo,sizeof(appinfo));		//Copy App Version
	length += sizeof(appinfo);

	memcpy(&txData[length],appsha,sizeof(appsha));			// Copy SHA of application
	length += sizeof(appsha);

	memcpy(&txData[length],&gFrameCount,sizeof(uint32_t));	// Copy Total Received Frame Count
	length += sizeof(uint32_t);

	txData[length] = BOARD_TYPE_LTC;
	length += 4;

	length += bus_rd_info_hook(&txData[length]);	// let application to add data to this frame

	*txLen = length;
	return RT_PROTO_OK;
}
#endif

#if (BUS_CAN_ENTER_BOOTLOADER == 1)
uint32_t enter_bootloader = 0;
extern void bus_entering_bootlaoder_hook();
uint32_t rt_bus_cmd_enter_bootloader(uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	enter_bootloader = 1; // Can not jump from interrupt context to bootloader application. Just flag the request
	return RT_PROTO_SkipReply;
}
#endif

void rt_bus_cmd_process()
{
#if (BUS_CAN_ENTER_BOOTLOADER == 1)
	if (enter_bootloader)
	{
		bus_entering_bootlaoder_hook();
		while(1)
		{
			bl_jump();
		}
	}
#endif

}

#if (BUS_CAN_WR_CALIBRATION == 1)
uint32_t rt_bus_cmd_cal_write_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{

	return RT_PROTO_OK;

}
#endif

#if (BUS_CAN_WR_CALIBRATION == 1)
uint32_t rt_bus_cmd_cal_read_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	return RT_PROTO_OK;
}
#endif

#if (BUS_CAN_BOOTLOADER_STAY == 1)
uint32_t gStayInBootloader = 0;

uint32_t rt_bus_cmd_bl_stay (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	gStayInBootloader = 1;
	return RT_PROTO_OK;
}

#endif

#if ((BUS_CAN_BOOTLOADER_WR == 1) || (BUS_CAN_BOOTLOADER_ERASE == 1))
extern const uint8_t aes_key[];

uint8_t flashBuffer[1024 +16];


#define MEM_TYPE_FLASH		0x00
#define MEM_TYPE_RAM		0x01
#define MEM_TYPE_UNK		0x03
uint32_t get_mem_type(uint32_t address)
{
	if ((address >= FLASH_START_ADDRESS) & (address<=FLASH_END_ADDRESS))
		return MEM_TYPE_FLASH;
	else if ((address >= RAM_START_ADDRESS) & (address <= RAM_END_ADDRESS))
		return MEM_TYPE_RAM;
	else
		return MEM_TYPE_UNK;
}
#endif

#if (BUS_CAN_BOOTLOADER_RD == 1)
uint32_t rt_bus_cmd_bl_read_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	return RT_PROTO_UnKownCommand;
}
#endif

#if (BUS_CAN_BOOTLOADER_WR == 1)
uint32_t rt_bus_cmd_bl_write_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	uint32_t writeaddress;
	uint32_t mtype;
	uint8_t decBuffer[16];
	uint8_t *iv;
	uint8_t *data;


	if (rxLen != 1060)
		return RT_PROTO_DataError;

	memcpy(&writeaddress,&rxData[0],4);

	if (writeaddress < APPLICATION_ADDRESS)
		return RT_PROTO_DataError;


	// Addr       IV      [ Magic     Data ]
	//  4         16      [ 16        1024 ]
	//				      [ Encrypted      ]
	// 0..3      4..19	  [ 20..23	..    1060 ]

	iv = &rxData[4];
	data = &rxData[20];

	AES_CBC_decrypt_buffer(decBuffer,data,16,aes_key,iv);
	if (memcmp("ROTA",&decBuffer[0],4) != 0)
		return RT_PROTO_DataError;
	if (memcmp("ROTA",&decBuffer[12],4) != 0)
		return RT_PROTO_DataError;


	AES_CBC_decrypt_buffer(flashBuffer,data,1040,aes_key,iv);

	mtype = get_mem_type(writeaddress);
	if (mtype == MEM_TYPE_FLASH)
	{
		FLASH_If_Init();
		if (FLASH_If_Write(writeaddress,(uint32_t *)&flashBuffer[16],1024/4) != HAL_OK)
			return RT_PROTO_ExcError;
#if THIS_BOARD_TYPE != BOARD_TYPE_PA
		dbg_printf("Write Done\n");
#endif
	}
	else if (mtype == MEM_TYPE_RAM)
	{
		memcpy((uint8_t *)writeaddress,&rxData[4],1024);
	}
	else
	{
		return RT_PROTO_DataError;
	}



	return RT_PROTO_OK;
}
#endif

#if (BUS_CAN_BOOTLOADER_ERASE == 1)
uint32_t rt_bus_cmd_bl_erase_handler (uint8_t *rxData,uint16_t rxLen,uint8_t *txData,uint16_t *txLen)
{
	uint32_t eraseaddress;
	uint32_t eraseLen;
	uint32_t mtype;

	if (rxLen != 0x08)
		return RT_PROTO_DataError;

	memcpy(&eraseaddress,&rxData[0],4);
	memcpy(&eraseLen,&rxData[4],4);

	if (eraseaddress % FLASH_PAGE_SIZE)
		return RT_PROTO_DataError;
	if (eraseLen % FLASH_PAGE_SIZE)
		return RT_PROTO_DataError;



	mtype = get_mem_type(eraseaddress);
	if (mtype == MEM_TYPE_FLASH)
	{
		FLASH_If_Init();
		if (FLASH_If_Erase(eraseaddress,eraseLen) != HAL_OK)
			return RT_PROTO_ExcError;
#if THIS_BOARD_TYPE != BOARD_TYPE_PA
		dbg_printf("Erase Done\n");
#endif
	}
	else if (mtype == MEM_TYPE_RAM)
	{
		memset((unsigned char *)eraseaddress,0,eraseLen);
	}
	else
	{
		return RT_PROTO_DataError;
	}

	return RT_PROTO_OK;
}
#endif

const tBusCommand commands[] =
{
#if (BUS_CAN_RD_SAMPLE == 1)
		{CMD_SMP_Read,			&rt_bus_cmd_smp_read_handler},
#endif
#if (BUS_CAN_WR_SAMPLE == 1)
		{CMD_SMP_Read,			&rt_bus_cmd_smp_write_handler},
#endif
#if (BUS_CAN_PING == 1)
		{CMD_PING, 				&rt_bus_cmd_ping_handler},
#endif
#if (BUS_CAN_IDENT == 1)
		{CMD_Ident, 			&rt_bus_cmd_ident_handler},
#endif
#if (BUS_CAN_RD_CONF == 1)
		{CMD_CONF_Read,			&rt_bus_cmd_conf_read_handler},
#endif
#if (BUS_CAN_WR_CONF == 1)
		{CMD_CONF_Write,		&rt_bus_cmd_conf_write_handler},
#endif
#if (BUS_CAN_WR_CALIBRATION == 1)
		{CMD_CalWrite,			&rt_bus_cmd_cal_write_handler},
#endif
#if (BUS_CAN_RD_CALIBRATION == 1)
		{CMD_CalRead,			&rt_bus_cmd_cal_read_handler},
#endif
#if (BUS_CAN_RD_INFO == 1)
		{CMD_Info_Read,			&rt_bus_cmd_read_info_handler},
#endif
#if (BUS_CAN_ENTER_BOOTLOADER == 1)
		{CMD_EnterBootloader, 	&rt_bus_cmd_enter_bootloader},
#endif
#if (BUS_CAN_RESET == 1)
		{CMD_RESET,				&rt_bus_cmd_reset},
#endif
#if (BUS_CAN_BOOTLOADER_STAY == 1)
		{CMD_BL_Stay, 			&rt_bus_cmd_bl_stay},
#endif
#if (BUS_CAN_BOOTLOADER_RD == 1)
		{CMD_BL_Read,			&rt_bus_cmd_bl_read_handler},
#endif
#if (BUS_CAN_BOOTLOADER_WR == 1)
		{CMD_BL_Write,			&rt_bus_cmd_bl_write_handler},
#endif
#if (BUS_CAN_BOOTLOADER_ERASE == 1)
		{CMD_BL_Erase,			&rt_bus_cmd_bl_erase_handler},
#endif

};

const int gCommandCount = (sizeof(commands)/sizeof(tBusCommand));
