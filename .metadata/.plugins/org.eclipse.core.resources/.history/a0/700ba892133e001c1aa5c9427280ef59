/******************************************************************************
 *
 *   Copyright (C) Rota Teknik 2019 All Rights Reserved. Confidential
 *
 **************************************************************************//**
 * @file        MAX31856.c
 * @brief       MAX31856 chip register definitions and function prototypes. There are different
 * 				version of thermocouple board.
 * 				1- 8 seperate channel. (nonmuxed board) Read/write sample or configuration directly
 * 				with MAX31856 per channel.
 * 				2- Muxed board. 2 MAX31856 chip which are connected to channel(GPIO) 4 and 6. Every
 * 				MAX31856 manages 4 different thermocouple channel.
 * 				3- Without external Cold Junction. Internal cold junction is used.
 * 				4- With external Cold Junction. External cold junction thermal sensor ST31 must be
 * 				configured and read external cold junction value from STS31 while reading
 * 				thermocouple sensor.
 *
 * @author      LeventK
 * @date        2017-07-31
 *
 * @ingroup     vts
 * @{
 *****************************************************************************/

/*============================================================================*/
/* Includes                                                                   */
/*============================================================================*/
#ifndef APPLICATION_USER_INC_MAX31586_H_
#define APPLICATION_USER_INC_MAX31586_H_

/*============================================================================*/
/* Forward declarations                                                       */
/*============================================================================*/

/*============================================================================*/
/* Constants and macros                                                       */
/*============================================================================*/
#define MAX_TC_CHANNELS  	4			/**< Max thermocouple channel */
#define TC_CH_SAMPLE_SIZE	6

#define MAX31856_CR0		0x00	/**< CR0 register address */
#define MAX31856_CR1		0x01	/**< CR1 register address */
#define MAX31856_MASK		0x02	/**< Fault Mask Register */
#define MAX31856_CJHF		0x03	/**< Cold Junction High Fault Threshold */
#define MAX31856_CJLF		0x04	/**< Cold Junction Low Fault Threshold */
#define MAX31856_LTHFTH		0x05	/**< Linearized Temperature High Fault Threshold MSB */
#define MAX31856_LTHFTL		0x06	/**< Linearized Temperature High Fault Threshold LSB */
#define MAX31856_LTLFTH		0x07	/**< Linearized Temperature Low Fault Threshold MSB */
#define MAX31856_LTLFTL		0x08	/**< Linearized Temperature High Fault Threshold MSB */
#define MAX31856_CJTO		0x09	/**< Cold Junction Offset Register */
#define MAX31856_CJTH		0x0A	/**< Cold Junction Temperature Register MSB */
#define MAX31856_CHTL		0x0B	/**< Cold Junction Temperature Register LSB */
#define MAX31856_LTCBH		0x0C	/**< Linearized TC Temperature Register Byte 2 */
#define MAX31856_LTCBM		0x0D	/**< Linearized TC Temperature Register Byte 1 */
#define MAX31856_LTCBL		0x0E	/**< Linearized TC Temperature Register Byte 0 */
#define MAX31856_SR			0x0F	/**< Fault Status Register */

#define CR0_50Hz			0x01	/**< 50Hz Sampling */
#define CR0_60Hz			0x00	/**< 60Hz Sampling */

#define CR0_FAULT_CLR		0x02	/**< Fault Status Clear */
#define CR0_FAULT			0x04	/**< Return Fault Status */
#define CR0_CJ_DIS			0x08	/**< Internal Cold Junction enable/disable */
#define CR0_OCFAULT0		0x10	/**< Fault bit */
#define CR0_OCFAULT1		0x20	/**< Fault bit */
#define CR0_1SHOT			0x40	/**< One shot mode */
#define CR0_CMODE_AUTO		0x80	/**< Automatic conversion mode */


#define CR1_TC_TYPE_B		0x00	/**< Thermocouple type B */
#define CR1_TC_TYPE_E		0x01	/**< Thermocouple type E */
#define CR1_TC_TYPE_J		0x02	/**< Thermocouple type J */
#define CR1_TC_TYPE_K		0x03	/**< Thermocouple type K */
#define CR1_TC_TYPE_N		0x04	/**< Thermocouple type N */
#define CR1_TC_TYPE_R		0x05	/**< Thermocouple type R */
#define CR1_TC_TYPE_S		0x06	/**< Thermocouple type S */
#define CR1_TC_TYPE_T		0x07	/**< Thermocouple type T */
#define CR1_TC_TYPE_VG8		0x08	/**< Thermocouple type Voltage Gain 8 */
#define CR1_TC_TYPE_VG32	0x0C	/**< Thermocouple type Voltage Gain 32 */

#define CR1_AVG_1			(0x00 << 4)	/**< Thermocouple Samples Averaged 1 */
#define CR1_AVG_2			(0x01 << 4)	/**< Thermocouple Samples Averaged 2 */
#define CR1_AVG_4			(0x02 << 4)	/**< Thermocouple Samples Averaged 4 */
#define CR1_AVG_8			(0x03 << 4)	/**< Thermocouple Samples Averaged 8 */
#define CR1_AVG_16			(0x04 << 4)	/**< Thermocouple Samples Averaged 16 */

#define SR_OPEN				0x01	/**< Status :  Open Circuit Fault */
#define SR_OVUV				0x02	/**< Status :  Over/Under Voltage Fault */
#define SR_TCLOW			0x04	/**< Status :  TC Low Threshold Fault */
#define SR_TCHIGH			0x08	/**< Status :  TC High Threshold Fault */
#define SR_CJLOW			0x10	/**< Status :  CJ Low Threshold Fault */
#define SR_CJHIGH			0x20	/**< Status :  CJ High Threshold Fault */
#define TC_RANGE			0x40	/**< Status :  TC Out of Range */
#define CJ_RANGE			0x80	/**< Status :  CJ Out of Range */

#define MAX31856_WR_REG		0x80	/**< Write Configuration Register */

#ifdef EXTCJ
#define CR0_DEFAULT	(CR0_50Hz + CR0_CMODE_AUTO  )
#else
#define CR0_DEFAULT	(CR0_50Hz + CR0_CMODE_AUTO + CR0_CJ_DIS)	/**< CR0 configuration register default values */
#endif

#define CR1_DEFAULT (CR1_TC_TYPE_T + CR1_AVG_16) /**< CR1 configuration register default values */

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/*============================================================================*/
/* Global data                                                                */
/*============================================================================*/

extern uint32_t g_MAX31856_Sample;

/*============================================================================*/
/* Module global data                                                         */
/*============================================================================*/

/*============================================================================*/
/* Implementation of functions                                                */
/*============================================================================*/


 /**
  * @brief Thermocouple init
  * @param extcj
  */
 void tc_init(int extcj);

 /**
  * @brief Read sensor value from channel (Unused)
  * @param ch	channel number
  * @param value	pointer to hold value
  * @return
  */
 uint8_t max31856_read(uint32_t ch,float *value);

 /**
  * @brief Copy sensor value from buffer that is read already
  * @param ch	channel number
  * @param raw	pointer to data
  */
 void max31856_read_raw_buffered(uint32_t ch,uint8_t *raw);

 /**
  * @brief Function is called by main in loop and it triggers to read sample (sensor value)
  * from MAX31856 chip.
  */
 void max31856_process();

 /**
  * @brief Read raw value from register (Unused)
  * @param ch	channel number
  * @param raw	pointer to data
  */
 void max31856_read_raw(uint32_t ch, uint8_t *raw);

 /**
  * @brief Read configuration from MAX31856 chip
  * @param ch	channel number
  * @param data	pointer to data
  */
 void max31856_read_conf(uint32_t ch, uint8_t *data);

 /**
  * @brief Write configuration to MAX31856 chip
  * @param ch	channel number
  * @param data	pointer to data
  */
 void max31856_write_conf(uint32_t ch, uint8_t *data);

#endif /* APPLICATION_USER_INC_MAX31586_H_ */
/**@}*/
/******************************************************************************/
/*   Copyright (C) Rota Teknik 2019,  All Rights Reserved. Confidential.      */
/******************************************************************************/
