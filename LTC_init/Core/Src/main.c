/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "debug.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SENSOR_TYPE_LSB 27

#define SENSOR_TYPE__TYPE_T_THERMOCOUPLE (uint32_t) 0x7 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__RTD_PT_100 (uint32_t) 0xC << SENSOR_TYPE_LSB
#define SENSOR_TYPE__SENSE_RESISTOR (uint32_t) 0x1D << SENSOR_TYPE_LSB

#define TC_COLD_JUNCTION_CH_LSB 22

#define TC_COLD_JUNCTION_CH__NONE (uint32_t) 0x0 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__12 (uint32_t) 0xC << TC_COLD_JUNCTION_CH_LSB

#define TC_DIFFERENTIAL_LSB 21
#define TC_DIFFERENTIAL (uint32_t) 0x0 << TC_DIFFERENTIAL_LSB
#define TC_SINGLE_ENDED (uint32_t) 0x1 << TC_DIFFERENTIAL_LSB

// tc - open ckt detect?
#define TC_OPEN_CKT_DETECT_LSB 20
#define TC_OPEN_CKT_DETECT__NO (uint32_t) 0x0 << TC_OPEN_CKT_DETECT_LSB
#define TC_OPEN_CKT_DETECT__YES (uint32_t) 0x1 << TC_OPEN_CKT_DETECT_LSB

// tc - open ckt detect current
#define TC_OPEN_CKT_DETECT_CURRENT_LSB 18
#define TC_OPEN_CKT_DETECT_CURRENT__10UA (uint32_t) 0x0 << TC_OPEN_CKT_DETECT_CURRENT_LSB
#define TC_OPEN_CKT_DETECT_CURRENT__100UA (uint32_t) 0x1 << TC_OPEN_CKT_DETECT_CURRENT_LSB
#define TC_OPEN_CKT_DETECT_CURRENT__500UA (uint32_t) 0x2 << TC_OPEN_CKT_DETECT_CURRENT_LSB
#define TC_OPEN_CKT_DETECT_CURRENT__1MA (uint32_t) 0x3 << TC_OPEN_CKT_DETECT_CURRENT_LSB

#define RTD_RSENSE_CHANNEL_LSB 22
#define RTD_RSENSE_CHANNEL__10 (uint32_t) 0xA << RTD_RSENSE_CHANNEL_LSB

#define RTD_NUM_WIRES_LSB 20
#define RTD_NUM_WIRES__4_WIRE (uint32_t) 0x2 << RTD_NUM_WIRES_LSB

#define RTD_EXCITATION_MODE_LSB 18
#define RTD_EXCITATION_MODE__NO_ROTATION_NO_SHARING (uint32_t) 0x0 << RTD_EXCITATION_MODE_LSB

#define RTD_EXCITATION_CURRENT_LSB 14
#define RTD_EXCITATION_CURRENT__25UA (uint32_t) 0x3 << RTD_EXCITATION_CURRENT_LSB

#define RTD_STANDARD_LSB 12
#define RTD_STANDARD__EUROPEAN (uint32_t) 0x0 << RTD_STANDARD_LSB

#define RSENSE_MAIN_VAL_LSB 10
#define RSENSE_2K (uint32_t) 0x7D0 << RSENSE_MAIN_VAL_LSB
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	if (GPIO_Pin == GPIO_PIN_11)
	{
		//dbg_printf("INT\r\n");
	}


}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t writeCmd = 0x02;
	uint8_t readCmd = 0x03;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  debug_init();

  uint8_t status = 0;
  uint8_t addr[2];
  addr[0] = 0x00;
  addr[1] = 0x00;


  for (int idx=0; idx<10; idx++)
  {
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&hspi1, &readCmd, 1, 1000);
	  HAL_SPI_Transmit(&hspi1, &addr[0], 2, 1000);
	  HAL_SPI_Receive(&hspi1, &status, 1, 1000);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	  HAL_Delay(100);

	  dbg_printf("Initial status : %x\n", status);

	  HAL_Delay(100);
  }

  uint32_t ch_data;
  ch_data = SENSOR_TYPE__TYPE_T_THERMOCOUPLE | TC_COLD_JUNCTION_CH__12 | TC_SINGLE_ENDED  | TC_OPEN_CKT_DETECT__YES | TC_OPEN_CKT_DETECT_CURRENT__10UA;

  uint8_t chDataArray[4];
  chDataArray[3] = (uint8_t)ch_data;
  chDataArray[2] = (uint8_t)(ch_data>>=8);
  chDataArray[1] = (uint8_t)(ch_data>>=8);
  chDataArray[0] = (uint8_t)(ch_data>>=8);

  for (int idx = 0; idx < 8; idx++)
  {
	  uint8_t chAddrArray[2];
	  chAddrArray[0] = 0x02;
	  chAddrArray[1] = 0x00 + 4*idx;

	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&hspi1, &writeCmd, 1, 1000);
	  HAL_SPI_Transmit(&hspi1, &chAddrArray[0], 2, 1000);
	  HAL_SPI_Transmit(&hspi1, &chDataArray[0], 4, 1000);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	  HAL_Delay(100);
  }

  uint32_t readMulti = 1<<0 | 1<<1 | 1<<2 |  1<<3  | 1<<4  | 1<<5  | 1<<6  | 1<<7  | 1<<8 /* | 1<<9 | 1<<10 | 1<<11| 1<<12 */| 1<<13 | 1<<14 | 1<<15 | 1<<16 | 1<<17 | 1<<18 | 1<<19 | 1<<20;
  uint8_t readDataMulti[4];
  readDataMulti[3] = (uint8_t)readMulti;
  readDataMulti[2] = (uint8_t)(readMulti>>=8);
  readDataMulti[1] = (uint8_t)(readMulti>>=8);
  readDataMulti[0] = (uint8_t)(readMulti>>=8);

  uint8_t readMultiAddr[2];
  readMultiAddr[0] = 0x00;
  readMultiAddr[1] = 0xF4;

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &writeCmd, 1, 1000);
  HAL_SPI_Transmit(&hspi1, &readMultiAddr[0], 2, 1000);
  HAL_SPI_Transmit(&hspi1, &readDataMulti[0], 4, 1000);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);























  uint32_t rtdData = SENSOR_TYPE__RTD_PT_100 | RTD_RSENSE_CHANNEL__10 | RTD_NUM_WIRES__4_WIRE | RTD_EXCITATION_MODE__NO_ROTATION_NO_SHARING | RTD_EXCITATION_CURRENT__25UA | RTD_STANDARD__EUROPEAN;
  uint8_t rtdDataArray[4];
  rtdDataArray[3] = (uint8_t)rtdData;
  rtdDataArray[2] = (uint8_t)(rtdData>>=8);
  rtdDataArray[1] = (uint8_t)(rtdData>>=8);
  rtdDataArray[0] = (uint8_t)(rtdData>>=8);

  uint8_t rtdAddr[2];
  rtdAddr[0] = 0x02;
  rtdAddr[1] = 0x2C;

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &writeCmd, 1, 1000);
  HAL_SPI_Transmit(&hspi1, &rtdAddr[0], 2, 1000);
  HAL_SPI_Transmit(&hspi1, &rtdDataArray[0], 4, 1000);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

  uint32_t rSenseData = SENSOR_TYPE__SENSE_RESISTOR | RSENSE_2K | (uint32_t)0x1E;
  uint8_t rSenseArray[4];
  rSenseArray[3] = (uint8_t)rSenseData;
  rSenseArray[2] = (uint8_t)(rSenseData>>=8);
  rSenseArray[1] = (uint8_t)(rSenseData>>=8);
  rSenseArray[0] = (uint8_t)(rSenseData>>=8);

  uint8_t rSenseAddr[2];
  rSenseAddr[0] = 0x02;
  rSenseAddr[1] = 0x24;

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &writeCmd, 1, 1000);
  HAL_SPI_Transmit(&hspi1, &rSenseAddr[0], 2, 1000);
  HAL_SPI_Transmit(&hspi1, &rSenseArray[0], 4, 1000);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);




















  int startTime = HAL_GetTick();
  uint8_t counter = 0;
  while (1)
  {
	  uint8_t beginConv = 0x80;
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&hspi1, &writeCmd, 1, 1000);
	  HAL_SPI_Transmit(&hspi1, &addr[0], 2, 1000);
	  HAL_SPI_Transmit(&hspi1, &beginConv, 1, 1000);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	  uint8_t process_fin = 0;

	  while(process_fin == 0)
	  {
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		  HAL_SPI_Transmit(&hspi1, &readCmd, 1, 1000);
		  HAL_SPI_Transmit(&hspi1, &addr[0], 2, 1000);
		  HAL_SPI_Receive(&hspi1, &status, 1, 1000);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

		  process_fin = status & 0x40;

	  }
	  /*HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 1);
	  HAL_Delay(500);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);
	  HAL_Delay(500);*/

	  for (int idx = 0; idx < 20; idx++)
	  {
		  uint8_t resAddrArray[2];
		  resAddrArray[0] = 0x00;
		  resAddrArray[1] = 0x10 + 4*idx;

		  uint8_t resArray[4];


		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		  HAL_SPI_Transmit(&hspi1, &readCmd, 1, 1000);
		  HAL_SPI_Transmit(&hspi1, &resAddrArray[0], 2, 1000);
		  HAL_SPI_Receive(&hspi1, &resArray[0], 4, 1000);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

		  //dbg_printf("Temperature : %x - %x - %x - %x \n", resArray[0], resArray[1], resArray[2], resArray[3]);

		  uint32_t rawData = (((uint32_t) resArray[0]) << 24 |
				  ((uint32_t) resArray[1]) << 16 |
				  ((uint32_t) resArray[2]) <<  8 |
				  ((uint32_t) resArray[3]) <<  0);

		  int statusTemp = rawData >> 24;

		  uint32_t rawConvData = rawData & 0xFFFFFF;

		  int signedRawData = rawConvData;

		  if(signedRawData & 0x800000)
			  signedRawData = signedRawData | 0xFF000000;

		  float scaledRes = (float)signedRawData / 1024;

		  if (idx == 0 || idx == 11)
		  {
			  dbg_printf("Ch : %d - Temperature : %f status : %x - %d\n", idx+1, scaledRes, statusTemp, HAL_GetTick());
			  dbg_printf("Ch : %d - Temperature : %f status : %x - %d\n", idx+1, scaledRes, statusTemp, HAL_GetTick());
		  }

	  }
	  counter++;

	  if (counter >=10)
	  {
		  dbg_printf("%d - %d\n", HAL_GetTick(), startTime);
		  counter = 0;
	  }


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
