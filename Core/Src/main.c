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
#include <stdint.h>
#include <string.h>
#include "stm32g4xx_hal.h"
#include "i2c.h"
#include "lsm9ds1.h"
#include "vector.h"
#include "magnet.h"
#include "gps.h"
#include "lcd.h"
#include "triad.h"
#include <inttypes.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define REarth16km 417542963

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
uint8_t rx_buffer[1]; // stores the data from every interrupt
RingBuffer uart_rx_buf = { .head = 0, .tail = 0 }; // stores every rx_buffer in a ring buffer, see interrupt function
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
  lsmCtrlReg(&hi2c3);
  GPSRead_t GPS = {0,0,0,0};
  HAL_Delay(5);
  //HAL_UART_Receive_IT(&huart1, rx_buffer, 1);
  LCD_Init();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  I2C_Scan(&hi2c3);

int32_t r = REarth16km + Rational(2,10); // Earth's radius in fixed point 16.16 format
printf("r : ");
printFix(r);
printf("\r\n");
int32_t phi = convert(56);
int32_t theta = convert(12);
int32_t days = 24510000;
int32_t vector[3];
magnet(r, theta, phi, days, vector);
int i;
for (i=0; i<3; i++) {
	printf("Res %i: ", i);
	printFix(vector[i]);
	printf("\r\n");
}

/*
Vector3D M2 = {     -(convert(100)>>5),  // North
	    			-(convert(3)>>5),    // East
					-(convert(166)>>5)}; //down
*/

//rigtig igrf fra NOAA, gør at lortet virker, 1/4 nT er enheden
Vector3D M2 = {     (17056<<14),  // North
					-(1464<<14),    // East
  					-(47628<<14)}; //down

/*
Vector3D M2 = {     (vector[0]>>8),  // North
					(vector[1]>>8),    // East
  					-(vector[2]>>8)}; //down
*/
Vector3D g2 = { 0, 0, 17000};

//her kan man rotere en matrix, dvs input forskel fra geografisk nord
//M2 = rotateZ14(&M2, convert(90));
//g2 = rotateZ14(&g2, convert(90));

	LCD_SetCursor(0, 0);
    LCD_SendString("Roll");
    LCD_SetCursor(0, 6);
    LCD_SendString("Pitch");
    LCD_SetCursor(0, 12);
    LCD_SendString("Yaw");

i = 0;
Vector3D degrot;
Matrix3x3 rot;
Matrix3x3 Rnb;
Vector3D acc_avg = {0, 0, convert(1)};
Vector3D mag_avg = {convert(1), 0, 0};
Vector3D accdata = {0,0,0};

//til at teste om sinus lut virker
printFix(sinrad(205887>>1));
printf("\n");
printFix(cosrad(205887>>1));
printf("\n");

  while (1)
  {
	  accdata = lsmAccRead(&hi2c3);

	  //printf("\n acc: %ld, %ld, %ld \n",accdata.x,accdata.y, accdata.z);
	  //printf("mag: %ld, %ld, %ld \n",magdata.x,magdata.y, magdata.z);


	  readSensorsAndAverage(&acc_avg, &mag_avg, hi2c3);
	  triad(mag_avg,acc_avg,M2,g2, &rot);
	  Rnb = transpose(rot);
	  rot2eulerZYX(&Rnb,&degrot);
	  //printf("\n %ld, %ld, %ld \n",degrot.x, degrot.y, degrot.z);

	  //printf("%ld,%ld,%ld\n", (long)mag_avg.x, (long)mag_avg.y, (long)mag_avg.z);
	  //printf("%ld,%ld,%ld\n", acc_avg.x, acc_avg.y, acc_avg.z);

	  //HAL_Delay(5);

	  //////////////////////////////////LCD deg print//////////////////
	  LCD_SetCursor(1, 0);
	  LCD_PrintAngle(inconvert(degrot.x));
	  HAL_Delay(5); //vigtigt der skal være delay ellers virker det ikke at rykke cursor

	  LCD_SetCursor(1, 6);
	  LCD_PrintAngle(inconvert(degrot.y));
	  HAL_Delay(5);

	  LCD_SetCursor(1, 12);
	  LCD_PrintAngle(inconvert(degrot.z));
	  HAL_Delay(5);
	  //////////////////////////////////LCD deg print end//////////////
	  static int32_t max_x = INT32_MIN, max_y = INT32_MIN, max_z = INT32_MIN;
	  static int32_t min_x = INT32_MAX, min_y = INT32_MAX, min_z = INT32_MAX;

	  /* bit1 = new-max, bit0 = new-min  -> 00 / 01 / 10 / 11 */
	  uint8_t fx = 0, fy = 0, fz = 0;

	  if (mag_avg.x > max_x) { max_x = mag_avg.x; fx |= 0b10; }
	  if (mag_avg.x < min_x) { min_x = mag_avg.x; fx |= 0b01; }

	  if (mag_avg.y > max_y) { max_y = mag_avg.y; fy |= 0b10; }
	  if (mag_avg.y < min_y) { min_y = mag_avg.y; fy |= 0b01; }

	  if (mag_avg.z > max_z) { max_z = mag_avg.z; fz |= 0b10; }
	  if (mag_avg.z < min_z) { min_z = mag_avg.z; fz |= 0b01; }

	  /* printable strings for each 2-bit flag value */
	  static const char* const flag_txt[4] = {"00","01","10","11"};

	  /* -------------- CSV log line ----------------------------------------- */
	  printf("%ld,%ld,%ld,%s,%s,%s\r\n",
	         (long)mag_avg.x, (long)mag_avg.y, (long)mag_avg.z,
	         flag_txt[fx], flag_txt[fy], flag_txt[fz]);

	  //process_uart_data(&uart_rx_buf, &GPS);
	  //HAL_Delay(5);
	  i++;
	  	  //if (!(i % 1000)) {
	  		  //i = 0;
	  		  //printGPS(GPS);
	  		  //printVector(lsmMagRead(&hi2c3));
	  		  //printf("\r\n");
	  	  //}
	}

	/*
	printFixVector(lsmMagOut(&hi2c3));
	printf("\r\n");
	printVector(lsmMagRead(&hi2c3));
	printf("\r\n");
	*/
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA4 PA5
                           PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

//
// code for setting printf to uart
//
extern UART_HandleTypeDef huart2;

/**
 * @brief This is necessary to write to the USB port with Putty
 */

int _write(int file, char *data, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)data, len, HAL_MAX_DELAY);
    return len;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart1) {
		//printf("UART Error: ISR=0x%08lX, SR=0x%08lX\r\n", huart->Instance->ISR, huart->Instance->RQR);
		 RingBuffer_Write(&uart_rx_buf, rx_buffer[0]);
		 HAL_UART_Receive_IT(&huart1, rx_buffer, 1);  // Re-arm
		 //printf("RX: %c (0x%02X)\r\n", rx_buffer[0], rx_buffer[0]);
	}
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

