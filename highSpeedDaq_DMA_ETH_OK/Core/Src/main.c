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
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "debug.h"
#include "dwt.h"
#include "ad7768.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SAMPLE_DMA_SIZE		1408
#define SAMPLE_SIZE			64

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;
DMA_HandleTypeDef hdma_spi1_rx;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI3_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

struct pbuf * p1;
struct pbuf * p2;

uint8_t EXTI0_Flag = 0;
uint8_t SPI_RxDMABuffer[SAMPLE_DMA_SIZE * 2 + 4 + 2] = { 0 };

static struct udp_pcb * test_pcb;
static uint16_t test_port = 55554; // arbitrary
static const ip_addr_t * test_destIp = IP_ADDR_BROADCAST; //or wherever your destination device is
extern ip4_addr_t ipaddr;
ip4_addr_t ipaddr_dest;

uint32_t extCnt = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	if (GPIO_Pin == GPIO_PIN_4) {
		__HAL_SPI_ENABLE(&hspi1);

		EXTI0_Flag = 1;

		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, extCnt % 2);
		extCnt++;
	}

}

uint8_t halfSend = 0;
uint8_t errUDP = 0;
void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
	SPI_RxDMABuffer[0]++;
	p1->payload = &SPI_RxDMABuffer[0];
	halfSend = 1;
/*
	errUDP = udp_sendto(test_pcb, p1, &ipaddr_dest, test_port);
	if (errUDP)
	{
		dbg_printf("error 1 : %d", errUDP);
	}
*/
}

uint8_t fullSend = 0;
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	SPI_RxDMABuffer[SAMPLE_DMA_SIZE * 2 + 4]++;
	p2->payload = &SPI_RxDMABuffer[SAMPLE_DMA_SIZE + 4];
	fullSend = 1;
/*
	errUDP = udp_sendto(test_pcb, p2, &ipaddr_dest, test_port);
	if (errUDP)
	{
		dbg_printf("error 2 : %d", errUDP);
	}
*/
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_SPI3_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  //MX_LWIP_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(ETH_RESET_GPIO_Port, ETH_RESET_Pin, RESET);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(ETH_RESET_GPIO_Port, ETH_RESET_Pin, SET);
  MX_LWIP_Init();

  debug_init();
  DWT_Init();
  SystemCoreClockUpdate();

  dbg_printf("Clock : %d\r\n", SystemCoreClock);

  AD7768_setup(AD7768_INTERFACE_CFG_DCLK_DIV(AD7768_DCLK_DIV_2),
		  AD7768_CH_MODE_FILTER_TYPE | AD7768_CH_MODE_DEC_RATE(AD7768_DEC_X128),
		  AD7768_PWR_MODE_POWER_MODE(
				  AD7768_FAST) | AD7768_PWR_MODE_MCLK_DIV(AD7768_MCLK_DIV_4));
  HAL_Delay(100);

  GPIO_InitTypeDef GPIO_InitStruct = { 0 };
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PD0 ADC_SYNC_START_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  test_pcb = udp_new();
  udp_bind(test_pcb, &ipaddr, 54321);
  p1 = pbuf_alloc(PBUF_TRANSPORT, SAMPLE_DMA_SIZE + 4, PBUF_REF);
  p1->payload = (uint8_t *)malloc((SAMPLE_DMA_SIZE + 4) * sizeof(uint8_t));
  memset(p1->payload, 0, SAMPLE_DMA_SIZE+4);

  p2 = pbuf_alloc(PBUF_TRANSPORT, SAMPLE_DMA_SIZE + 2, PBUF_REF);
  p2->payload = (uint8_t *)malloc((SAMPLE_DMA_SIZE + 2) * sizeof(uint8_t));
  memset(p2->payload, 0, SAMPLE_DMA_SIZE + 2);


  uint8_t IP_ADDRESS_DEST[4];
  IP_ADDRESS_DEST[0] = 192;
  IP_ADDRESS_DEST[1] = 168;
  IP_ADDRESS_DEST[2] = 99;
  IP_ADDRESS_DEST[3] = 121;

  IP4_ADDR(&ipaddr_dest, IP_ADDRESS_DEST[0], IP_ADDRESS_DEST[1], IP_ADDRESS_DEST[2], IP_ADDRESS_DEST[3]);

  uint8_t initial = 1;

  while (1) {
	  /* USER CODE END WHILE */

	  /* USER CODE BEGIN 3 */

	  MX_LWIP_Process();

	  if (halfSend == 1)
	  {
		  errUDP = udp_sendto(test_pcb, p1, &ipaddr_dest, test_port);
		  if (errUDP)
		  {
			  dbg_printf("error 1 : %d", errUDP);
		  }
		  halfSend = 0;
	  }
	  if (fullSend == 1)
	  {
		  errUDP = udp_sendto(test_pcb, p2, &ipaddr_dest, test_port);
		  if (errUDP)
		  {
			  dbg_printf("error 2 : %d", errUDP);
		  }
		  fullSend = 0;
	  }
	  if (halfSend && fullSend)
	  {
		  dbg_printf("Error\n");
	  }
	  if (errUDP)
	  {
		  dbg_printf("error 3 : %d", errUDP);
	  }

	  if (initial == 1)
	  {
		  __HAL_DMA_ENABLE_IT(&hdma_spi1_rx, DMA_IT_HT);
		  __HAL_DMA_ENABLE_IT(&hdma_spi1_rx, DMA_IT_TC);
		  HAL_SPI_Receive_DMA(&hspi1, &SPI_RxDMABuffer[4], SAMPLE_DMA_SIZE * 2);
		  initial = 0;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV5;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL8;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the Systick interrupt time
  */
  __HAL_RCC_HSE_PREDIV2_CONFIG(RCC_HSE_PREDIV2_DIV5);
  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_CONFIG(RCC_PLLI2S_MUL10);
  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
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
  hspi1.Init.Mode = SPI_MODE_SLAVE;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_INPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

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
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, ETH_RESET_Pin|GPIO_PIN_0|ADC_SYNC_START_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ETH_RESET_Pin PD0 ADC_SYNC_START_Pin */
  GPIO_InitStruct.Pin = ETH_RESET_Pin|GPIO_PIN_0|ADC_SYNC_START_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

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
	while (1) {
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
