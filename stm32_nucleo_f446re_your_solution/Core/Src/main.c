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
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//The AS5047 requires at least 350ns between the LOW CS and the begin of the communication
#define AS5047_CS_LOW	{HAL_GPIO_WritePin(GPIOB, AS5047_CS_Pin, GPIO_PIN_RESET);  HAL_Delay(1);}
#define AS5047_CS_HIGH	HAL_GPIO_WritePin(GPIOB, AS5047_CS_Pin, GPIO_PIN_SET);


#define OP_READ   	      0x4000
#define PARITY_EVEN       0x0000
#define PARITY_ODD        0x8000
#define ADDR_NOP  	      0x0000
#define ADDR_ERRFL 	      0x0001
#define ADDR_ANGLECOM     0x3FFF

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint16_t spi_buf_send; 		//store the message the to be sent though SPI
  uint16_t spi_buf_receive;   //store the message the to be received though SPI
  char uart_buf[50];		//store the message the to be sent though UART
  uint16_t uart_buf_len;

  uint16_t CMD_R_ANGLECOM = (PARITY_ODD  | OP_READ | ADDR_ANGLECOM);
  uint16_t CMD_R_NOP      = (PARITY_ODD  | OP_READ | ADDR_NOP);
  uint16_t CMD_R_ERRFL    = (PARITY_EVEN | OP_READ | ADDR_ERRFL);

  HAL_StatusTypeDef status;

  float angle;

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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  AS5047_CS_HIGH;

  //This section was translated from the original Arduino code. However, this section doesn't work because
  //0x0001 the the error register and it is only read

//  spi_buf_send = 0x0001; //writing command on the error register
//  AS5047_CS_LOW;
//  HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&spi_buf_send, (uint8_t*)&spi_buf_receive, 2, 100);
//  AS5047_CS_HIGH;
//  uart_buf_len = sprintf(uart_buf, "%d\r\n", spi_buf_receive); //print data
//  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, uart_buf_len, 100);
//
//  spi_buf_send = 0x0007; //this is the value written on the error register
//  AS5047_CS_LOW;
//  HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&spi_buf_send, (uint8_t*)&spi_buf_receive, 2, 100);
//  AS5047_CS_HIGH;
//  uart_buf_len = sprintf(uart_buf, "%d\r\n", spi_buf_receive); //print data
//  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, uart_buf_len, 100);


  spi_buf_send = CMD_R_ERRFL;
  spi_buf_receive=0;
  AS5047_CS_LOW;
  HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&spi_buf_send, (uint8_t*)&spi_buf_receive, 1, 100);
  AS5047_CS_HIGH;
  //The received message is trash
//  uart_buf_len = sprintf(uart_buf, "%d\r\n", spi_buf_receive); //print data
//  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, uart_buf_len, 100);

  spi_buf_send = CMD_R_NOP;
  spi_buf_receive=0;
  AS5047_CS_LOW;
  HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&spi_buf_send, (uint8_t*)&spi_buf_receive, 1, 100);
  AS5047_CS_HIGH;
  uart_buf_len = sprintf(uart_buf, "%d\r\n", spi_buf_receive); //print data
  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, uart_buf_len, 100);

  if(spi_buf_receive != 0){
	  uart_buf_len = sprintf(uart_buf, "Error %d Detected\r\n", spi_buf_receive); //print data
	  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, uart_buf_len, 100);
  }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  //send the address of the angle register
	  AS5047_CS_LOW;
	  HAL_SPI_Transmit(&hspi1, (uint8_t*)&CMD_R_ANGLECOM, 1, 100);
	  AS5047_CS_HIGH;

	  //send NOP and read the angle
	  AS5047_CS_LOW;
	  spi_buf_receive=0;
	  status = HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&CMD_R_NOP, (uint8_t*)&spi_buf_receive, 1, 100);
	  AS5047_CS_HIGH

	  if(((spi_buf_receive &0x4000)==0) && (status==HAL_OK)){
		  //removing the status bits
		  spi_buf_receive &= 0x3FFF;

		  uart_buf_len = sprintf(uart_buf, "Raw value = %d\r\n", spi_buf_receive); //print raw data
		  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, uart_buf_len, 100);

		  angle = ((float)spi_buf_receive/16384.0 ) *360.0;
		  uart_buf_len = sprintf(uart_buf, "The angle is = %0.2f\r\n",angle); //print the angle
		  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, uart_buf_len, 100);
		  }

	  HAL_Delay(100);


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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
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
  huart2.Init.Mode = UART_MODE_TX_RX;
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(AS5047_CS_GPIO_Port, AS5047_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : AS5047_CS_Pin */
  GPIO_InitStruct.Pin = AS5047_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(AS5047_CS_GPIO_Port, &GPIO_InitStruct);

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
