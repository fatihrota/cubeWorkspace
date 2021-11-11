/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2019 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        ext_cj.c
 * @brief       External Cold Junction implementation.
 *
 * @author      LeventK
 * @date        2018-07-10
 *
 * @ingroup     vts
 * @{
 *****************************************************************************/

/*============================================================================*/
/* Includes                                                                   */
/*============================================================================*/

#include "ext_cj.h"
#include "main.h"

/*============================================================================*/
/* Forward declarations                                                       */
/*============================================================================*/

/*============================================================================*/
/* Constants and macros                                                       */
/*============================================================================*/

#define EXT_THERMAL_COUNT	1		/**< External Cold Junction count */
#define SOFTI2C_WAIT		5

#define SOFT_I2C_PORT	GPIOB		/**< Soft I2C Configuration */
#define SOFT_I2C_SCL    GPIO_PIN_6
#define SOFT_I2C_SDA    GPIO_PIN_7

#define CRC8INIT 0xFF	/**< CRC definitions */
#define CRC8POLY 0x31 	/**< X^8+X^5+X^4+X^0 */

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/**
 * SoftI2C struct definition
 */
typedef struct
{
	GPIO_TypeDef *port;
	uint16_t sda;
	uint16_t scl;
	uint32_t mode;
	uint32_t pull;
	uint32_t speed;
}tSoftI2C;

/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

const uint8_t STS_ADDR = 0x4A;

static const tSoftI2C gSoftIC2 =
{
		.port = SOFT_I2C_PORT,
		.sda = SOFT_I2C_SDA,
		.scl = SOFT_I2C_SCL,
		.mode = GPIO_MODE_AF_OD,
		.pull = GPIO_PULLUP,
		.speed = GPIO_SPEED_FREQ_HIGH,

};

/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/

/**
 * @brief Delay for specified period
 * @param micros	delay time in microseconds
 */
void Delay(uint32_t micros)
{
	  /* Go to clock cycles */
	  	micros *= (SystemCoreClock / 1000000) / 5;

	  	/* Wait till done */
	  	while (micros--);
}

/*****************************************************************************/
/**
 * @brief Sleep required for softI2C implementation
 * @param useconds	delay time in microseconds
 */
void softI2C_sleep_usec(uint32_t useconds)
{
	Delay(useconds);
}

/*****************************************************************************/
/**
 * @brief Read SCL pin and return
 * @return
 */
uint8_t softI2C_SCL_read(void)
{
    return (uint8_t)HAL_GPIO_ReadPin(gSoftIC2.port, gSoftIC2.scl) == GPIO_PIN_SET;
}

/*****************************************************************************/
/**
 * @brief Read SDA pin and return
 * @return
 */
uint8_t softI2C_SDA_read(void)
{
    return (uint8_t)HAL_GPIO_ReadPin(gSoftIC2.port, gSoftIC2.sda) == GPIO_PIN_SET;
}

/*****************************************************************************/
/**
 * @brief Wait for SoftI2C
 * @return 0 or -1
 */
static uint8_t softI2C_wait_while_clock_stretching(void)
{
    uint8_t timeout = 100;

    while (--timeout) {
        if (softI2C_SCL_read())
            return 0;
        softI2C_sleep_usec(SOFTI2C_WAIT);
    }

    return -1;
}

/*****************************************************************************/
/**
 * @brief Change SDA mode input
 */
void softI2C_SDA_in(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = gSoftIC2.sda,
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_HIGH,
    };
    HAL_GPIO_Init(gSoftIC2.port, &GPIO_InitStruct);
}

/*****************************************************************************/
/**
 * @brief Change SDA mode output
 */
void softI2C_SDA_out(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = gSoftIC2.sda,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_HIGH,
    };
    HAL_GPIO_Init(gSoftIC2.port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(gSoftIC2.port, gSoftIC2.sda, GPIO_PIN_RESET);
}

/*****************************************************************************/
/**
 * @brief Change SCL mode input
 */
void softI2C_SCL_in(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = gSoftIC2.scl,
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_HIGH,
    };
    HAL_GPIO_Init(gSoftIC2.port, &GPIO_InitStruct);
}

/*****************************************************************************/
/**
 * @brief Change SCL mode output
 */
void softI2C_SCL_out(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = gSoftIC2.scl,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_HIGH,
    };
    HAL_GPIO_Init(gSoftIC2.port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(gSoftIC2.port, gSoftIC2.scl, GPIO_PIN_RESET);
}

/*****************************************************************************/
/**
 * @brief Soft I2C start
 * @return	-1 or 0
 */
static uint8_t softI2C_start(void)
{
    softI2C_SCL_in();
    if (softI2C_wait_while_clock_stretching())
        return -1;

    softI2C_SDA_out();
    softI2C_sleep_usec(SOFTI2C_WAIT);
    softI2C_SCL_out();
    softI2C_sleep_usec(SOFTI2C_WAIT);
    return 0;
}

/*****************************************************************************/
/**
 * @brief Soft I2C stop
 */
static void softI2C_stop(void)
{
	softI2C_SDA_out();
	softI2C_sleep_usec(SOFTI2C_WAIT);
    softI2C_SCL_in();
    softI2C_sleep_usec(SOFTI2C_WAIT);
    softI2C_SDA_in();
    softI2C_sleep_usec(SOFTI2C_WAIT);
}

/*****************************************************************************/
/**
 * @brief Fill uint8_t buffer with uint16_t command
 * @param buf		buffer to be filled
 * @param cmd		command
 * @param args		argument	(Unusued)
 * @param num_args	number of arguments	(Unused)
 * @return	index in buffer
 */
uint16_t softI2C_fill_cmd_send_buf(uint8_t *buf, uint16_t cmd,
                                     const uint16_t *args, uint8_t num_args)
{
    uint16_t idx = 0;

    buf[idx++] = (uint8_t)((cmd & 0xFF00) >> 8);
    buf[idx++] = (uint8_t)((cmd & 0x00FF) >> 0);

    return idx;
}

/*****************************************************************************/
/**
 * @brief SoftI2C Write command to STS31 thermal sensor chip
 * @param data	data to be sent
 * @return	-1 or 0
 */
int STS31_WriteCmd_soft(uint8_t data)
{
	int8_t nack, i;
	for (i = 7; i >= 0; i--)
	{
		softI2C_SCL_out();
		if ((data >> i) & 0x01)
			softI2C_SDA_in();
		else
			softI2C_SDA_out();
		softI2C_sleep_usec(SOFTI2C_WAIT);
		softI2C_SCL_in();
		softI2C_sleep_usec(SOFTI2C_WAIT);
		if (softI2C_wait_while_clock_stretching())
			return -1;
	}
	softI2C_SCL_out();
	softI2C_SDA_in();
	softI2C_sleep_usec(SOFTI2C_WAIT);
	softI2C_SCL_in();
	if (softI2C_wait_while_clock_stretching())
		return -1;
	nack = (softI2C_SDA_read() != 0);
	softI2C_SCL_out();

	return 0;
}

/*****************************************************************************/
/**
 * @brief SoftI2C Read command from STS31 thermal sensor chip
 * @param ack	send ack or not
 * @return	data or -1
 */
int STS31_ReadCmd_soft(uint8_t ack)
{
	int8_t i;
	uint8_t data = 0;
	softI2C_SDA_in();
	for (i = 7; i >= 0; i--)
	{
		softI2C_sleep_usec(SOFTI2C_WAIT);
		softI2C_SCL_in();
		if (softI2C_wait_while_clock_stretching())
			return -1;
		data |= (softI2C_SDA_read() != 0) << i;
		softI2C_SCL_out();
	}
	if (ack)
		softI2C_SDA_out();
	else
		softI2C_SDA_in();
	softI2C_sleep_usec(SOFTI2C_WAIT);
	softI2C_SCL_in();
	softI2C_sleep_usec(SOFTI2C_WAIT);
	if (softI2C_wait_while_clock_stretching())
		return -1;
	softI2C_SCL_out();
	softI2C_SDA_in();

	return data;
}

/*****************************************************************************/
/**
 * @brief SoftI2C Write data to address
 * @param address	write address
 * @param cmd		command
 * @param count		unused
 * @return	-1 or return value
 */
int8_t softI2C_i2c_write(uint8_t address, const uint16_t cmd,
                           uint16_t count)
{
    int8_t ret;
    uint8_t buf[2];

    softI2C_fill_cmd_send_buf(buf, cmd, NULL, 0);

    ret = softI2C_start();
    if (ret != 0)
        return -1;

    ret = STS31_WriteCmd_soft(address << 1 | 0);
    if (ret != 0) {
        softI2C_stop();
        return -1;
    }

    for (int i = 0; i < 2; i++)
    {
    	ret = STS31_WriteCmd_soft(buf[i]);
    }

    softI2C_stop();
    return ret;
}

/*****************************************************************************/
/**
 * @brief STS31 crc check (unused)
 * @param data
 * @param size
 * @return
 */
uint8_t STS31_crc8(uint8_t *data, uint16_t size)
{
    uint8_t crc, i;
    crc = CRC8INIT;

    while (size--)
    {
        crc ^= *data++;

        for (i = 0; i < 8; i++)
        {
            if (crc & 0x80) crc = (crc << 1) ^ CRC8POLY;
            else crc <<= 1;
        }
    }

    return crc;
}

/*****************************************************************************/
/**
 * @brief SoftI2C Read data from address
 * @param address	read address
 * @param cmd		command
 * @param data		pointer to read data buffer
 * @param count		(unusued)
 * @return	0 or return value
 */
int8_t softI2C_i2c_read(uint8_t address, uint16_t cmd, uint16_t *data, uint16_t count)
{
    int8_t ret;
    uint8_t v[2];

    softI2C_i2c_write(address, cmd, 1);

    ret = softI2C_start();
    if (ret != 0)
        return ret;

    ret = STS31_WriteCmd_soft((address << 1) | 1);
    if (ret != 0) {
    	softI2C_stop();
        return ret;
    }

    v[0] = STS31_ReadCmd_soft(1);
    v[1] = STS31_ReadCmd_soft(1);
    uint8_t c = STS31_ReadCmd_soft(0);

    *data= (((uint16_t )v[0])<<8) | v[1];

    softI2C_stop();
    return 0;
}

/*****************************************************************************/
/**
 * @brief STS31 Reset
 * @param ch	Select STS31 thermal sensor chip
 * @return
 */
int STS31_SoftReset_soft()
{
	return softI2C_i2c_write(STS_ADDR,0x30A2,1);
}

/*****************************************************************************/
/**
 * @brief STS31 Periodic Mode
 * @param ch	Select STS31 thermal sensor chip
 * @return
 */
int STS31_EnterPeriodicMode_soft()
{
	return softI2C_i2c_write(STS_ADDR,0x2032,1);
}

/*****************************************************************************/
/**
 * @brief Initialize softI2C
 * @return	external Cold Junction STS31 thermal sensor count
 */
int EXTCJ_ll_init_soft(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	uint8_t extCJ_count = 0;
	softI2C_SDA_in();
	softI2C_SCL_in();

	for(int i=0; i<EXT_THERMAL_COUNT; i++)
	{
		if (STS31_SoftReset_soft(i) < 0)
		{
			return 0;
		}
		HAL_Delay(50);
		if (STS31_EnterPeriodicMode_soft(i) < 0)
		{
			return 0;
		}

		extCJ_count++;
	}

	return extCJ_count;
}

/*****************************************************************************/
int EXTCJ_init(void)
{
	if (EXTCJ_ll_init_soft() < EXT_THERMAL_COUNT)
	{
		return -1;
	}

	return EXT_THERMAL_COUNT;
}

int EXTCJ_ReadTemperature(float *data)
/*****************************************************************************/
{
	uint16_t t;
	int r;
	softI2C_i2c_write(STS_ADDR,0x2032,1);	/**< Fetch Temperature Command */
	HAL_Delay(5);
	r=softI2C_i2c_read(STS_ADDR,0xE000,&t,1);	/**< Read Temperature Data */
	if (r != 0)
		return r;
	*data = (((float)t * 175.0f) / 65535.0f) - 45.0f;

	return 0;
}


/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2019,  All Rights Reserved. Confidential.      */
/******************************************************************************/

