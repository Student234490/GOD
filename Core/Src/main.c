/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "stm32g4xx_hal.h"
#include "i2c.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LSM9DS1_MAG_ADDRESS   0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_ACC_ADDRESS   0x6B

#define OUT_X_L_M   0x28 //
#define OUT_Y_L_M   0x2A //
#define OUT_Z_L_M   0x2C //

#define OUT_X_XL   0x28 //
#define OUT_Y_XL   0x2A //
#define OUT_Z_XL   0x2C //


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C3_Init(void);
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
  MX_USART1_UART_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */
  // Enable accelerometer: ODR = 119 Hz, ±2g, BW = 50 Hz

  uint8_t acc_ctrl_reg8[2] = {0x22, 0x05}; // reboot + soft reset
  HAL_I2C_Master_Transmit(&hi2c3, LSM9DS1_ACC_ADDRESS << 1, acc_ctrl_reg8, 2, HAL_MAX_DELAY);
  HAL_Delay(100);

  uint8_t acc_ctrl_reg6[2] = {0x20, 0x60}; // 0x60 = 0b01100000 → ODR = 119Hz, ±2g
  HAL_I2C_Master_Transmit(&hi2c3, LSM9DS1_ACC_ADDRESS << 1, acc_ctrl_reg6, 2, HAL_MAX_DELAY);
  HAL_Delay(10);

  uint8_t mag_ctrl_reg1[2] = {0x20, 0b01110000}; //
  HAL_I2C_Master_Transmit(&hi2c3, LSM9DS1_MAG_ADDRESS << 1, mag_ctrl_reg1, 2, HAL_MAX_DELAY);
  HAL_Delay(10);

  uint8_t mag_ctrl_reg3[2] = {0x22, 0b00000000}; //
  HAL_I2C_Master_Transmit(&hi2c3, LSM9DS1_MAG_ADDRESS << 1, mag_ctrl_reg3, 2, HAL_MAX_DELAY);
  HAL_Delay(10);

  uint8_t mag_ctrl_reg4[2] = {0x23, 0b00001100}; //
  HAL_I2C_Master_Transmit(&hi2c3, LSM9DS1_MAG_ADDRESS << 1, mag_ctrl_reg4, 2, HAL_MAX_DELAY);
  HAL_Delay(10);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  I2C_Scan(&hi2c3);
  while (1)
  {
	  //printf("Loop begun");


	  // Magnetometer example code
	  /*uint16_t buffer;
	  HAL_I2C_Master_Transmit(&hi2c3, LSM9DS1_MAG_ADDRESS << 1, OUT_X_L_M, 1, HAL_MAX_DELAY);
	  HAL_I2C_Master_Receive(&hi2c3, LSM9DS1_MAG_ADDRESS << 1 | 0x01, buffer, sizeof(buffer), HAL_MAX_DELAY);
	  printf("%d\n", buffer);
	  */
	  uint8_t reg1 = OUT_X_XL | 0x80; // Enable auto-increment
	  uint8_t data1[6];

	  // Request starting from OUT_X_XL with auto-increment
	  HAL_I2C_Master_Transmit(&hi2c3, LSM9DS1_ACC_ADDRESS << 1, &reg1, 1, HAL_MAX_DELAY);
	  HAL_I2C_Master_Receive(&hi2c3, (LSM9DS1_ACC_ADDRESS << 1) | 0x01, data1, 6, HAL_MAX_DELAY);

	  // Combine bytes into signed 16-bit integers
	  int16_t acc_x = (int16_t)(data1[1] << 8 | data1[0]);
	  int16_t acc_y = (int16_t)(data1[3] << 8 | data1[2]);
	  int16_t acc_z = (int16_t)(data1[5] << 8 | data1[4]);

	  uint8_t reg2 = OUT_X_L_M | 0x80; // Enable auto-increment
	  uint8_t data2[6];

	  // Request starting from OUT_X_XL with auto-increment
	  HAL_I2C_Master_Transmit(&hi2c3, LSM9DS1_MAG_ADDRESS << 1, &reg2, 1, HAL_MAX_DELAY);
	  HAL_I2C_Master_Receive(&hi2c3, (LSM9DS1_MAG_ADDRESS << 1) | 0x01, data2, 6, HAL_MAX_DELAY);

	  // Combine bytes into signed 16-bit integers
	  int16_t mag_x = (int16_t)(data2[1] << 8 | data2[0]);
	  int16_t mag_y = (int16_t)(data2[3] << 8 | data2[2]);
	  int16_t mag_z = (int16_t)(data2[5] << 8 | data2[4]);

	  printf("%d,%d,%d,%d,%d,%d,\n\r", mag_x, mag_y, mag_z, acc_x,acc_y,acc_z);
	  HAL_Delay(1000);

	  // GPS EXAMPLE CODE
	  /*uint8_t rx_byte;
	  while (1)
	  {
	      if (HAL_UART_Receive(&huart1, &rx_byte, 1, 100) == HAL_OK)
	      {
	          // Echo byte to USART2 for debugging
	    	  HAL_UART_Transmit(&huart2, &rx_byte, 1, HAL_MAX_DELAY);
	      }
	  }*/

	  //printf("Hello from STM32!\r\n");

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
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
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x0010061A;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

//
// code for setting printf to uart
//
extern UART_HandleTypeDef huart2;

int _write(int file, char *data, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)data, len, HAL_MAX_DELAY);
    return len;
}

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

