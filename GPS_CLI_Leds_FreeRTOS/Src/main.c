/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stdlib.h"
#include "../Src/libs/GPS.h"
#include "interfaces/uartGPS.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart6;

uint8 tempData;

DMA_HandleTypeDef hdma_usart4_tx;

osThreadId TargetLedTaskHandle;
osThreadId YellowLedTaskHandle;
osThreadId BlueLedTaskHandle;
osThreadId RedLedTaskHandle;
osThreadId GreenLedTaskHandle;
osThreadId CliTaskHandle;
osThreadId GPSTaskHandle;
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART4_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_USART2_UART_Init(void);
void TargetLedTaskFun(void const * argument);
void YellowLedTaskFun(void const * argument);
void BlueLedTaskFun(void const * argument);
void RedLedTaskFun(void const * argument);
void GreenLedTaskFun(void const * argument);
void CliTaskFun(void const * argument);
void GPSTaskFun(void const * argument);

static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

int len = 0;
char Rx_indx_CLI, Rx_data_CLI, Rx_Buffer_CLI[100];
char Transfer_cplt_CLI = 0;

char Rx_indx_gps1, Rx_data_gps1, Rx_Buffer_gps1[100];
char Transfer_cplt_gps1 = 0;

extern char Transfer_cplt_gps;


PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	HAL_UART_Transmit(&huart4, (uint8_t *)&ch, 1, 100);
	return ch;
}	

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
  MX_DMA_Init();
  MX_USART4_UART_Init();
  MX_USART6_UART_Init();
  MX_USART2_UART_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of TargetLedTask */
  osThreadDef(TargetLedTask, TargetLedTaskFun, osPriorityLow, 0, 128);
  TargetLedTaskHandle = osThreadCreate(osThread(TargetLedTask), NULL);

  /* definition and creation of YellowLedTask */
  osThreadDef(YellowLedTask, YellowLedTaskFun, osPriorityLow, 0, 128);
  YellowLedTaskHandle = osThreadCreate(osThread(YellowLedTask), NULL);

  /* definition and creation of BlueLedTask */
  osThreadDef(BlueLedTask, BlueLedTaskFun, osPriorityLow, 0, 128);
  BlueLedTaskHandle = osThreadCreate(osThread(BlueLedTask), NULL);

  /* definition and creation of RedLedTask */
  osThreadDef(RedLedTask, RedLedTaskFun, osPriorityLow, 0, 128);
  RedLedTaskHandle = osThreadCreate(osThread(RedLedTask), NULL);

  /* definition and creation of GreenLedTask */
  osThreadDef(GreenLedTask, GreenLedTaskFun, osPriorityLow, 0, 128);
  GreenLedTaskHandle = osThreadCreate(osThread(GreenLedTask), NULL);

  /* definition and creation of CliTask */
  osThreadDef(CliTask, CliTaskFun, osPriorityLow, 0, 128);
  CliTaskHandle = osThreadCreate(osThread(CliTask), NULL);

  /* definition and creation of GPSTask */
  osThreadDef(GPSTask, GPSTaskFun, osPriorityHigh, 0, 600);
  GPSTaskHandle = osThreadCreate(osThread(GPSTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI4_15_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
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
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
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
  * @brief USART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART4_UART_Init(void)
{

  /* USER CODE BEGIN USART4_Init 0 */

  /* USER CODE END USART4_Init 0 */

  /* USER CODE BEGIN USART4_Init 1 */

  /* USER CODE END USART4_Init 1 */
  huart4.Instance = USART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART4_Init 2 */

  /* USER CODE END USART4_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Ch2_3_DMA2_Ch1_2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Ch2_3_DMA2_Ch1_2_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, TargetLed_Pin|RedLed_Pin|GreenLed_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, YellowLed_Pin|BlueLed_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PushButton_Pin PPS_Pin */
  GPIO_InitStruct.Pin = PushButton_Pin|PPS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : TargetLed_Pin RedLed_Pin GreenLed_Pin */
  GPIO_InitStruct.Pin = TargetLed_Pin|RedLed_Pin|GreenLed_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : YellowLed_Pin BlueLed_Pin */
  GPIO_InitStruct.Pin = YellowLed_Pin|BlueLed_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_TargetLedTaskFun */
/**
  * @brief  Function implementing the TargetLedTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_TargetLedTaskFun */
void TargetLedTaskFun(void const * argument)
{

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
    for(;;)
    {
        //osDelay(2);
        vTaskDelay(1000);
        HAL_GPIO_TogglePin(TargetLed_GPIO_Port , TargetLed_Pin); 
    }
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_YellowLedTaskFun */
/**
* @brief Function implementing the YellowLedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_YellowLedTaskFun */
void YellowLedTaskFun(void const * argument)
{
  /* USER CODE BEGIN YellowLedTaskFun */
  /* Infinite loop */
    for(;;)
    {
        vTaskDelay(1);
        //HAL_GPIO_TogglePin(YellowLed_GPIO_Port , YellowLed_Pin); 
    }
  /* USER CODE END YellowLedTaskFun */
}

/* USER CODE BEGIN Header_BlueLedTaskFun */
/**
* @brief Function implementing the BlueLedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BlueLedTaskFun */
void BlueLedTaskFun(void const * argument)
{
  /* USER CODE BEGIN BlueLedTaskFun */
    /* Infinite loop */
    for(;;)
    {
        vTaskDelay(125);
        HAL_GPIO_TogglePin(BlueLed_GPIO_Port , BlueLed_Pin); 
    }
  /* USER CODE END BlueLedTaskFun */
}

/* USER CODE BEGIN Header_RedLedTaskFun */
/**
* @brief Function implementing the RedLedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_RedLedTaskFun */
void RedLedTaskFun(void const * argument)
{
  /* USER CODE BEGIN RedLedTaskFun */
    /* Infinite loop */
    for(;;)
    {
        vTaskDelay(250);
        HAL_GPIO_TogglePin(RedLed_GPIO_Port , RedLed_Pin); 
    }
  /* USER CODE END RedLedTaskFun */
}

/* USER CODE BEGIN Header_GreenLedTaskFun */
/**
* @brief Function implementing the GreenLedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_GreenLedTaskFun */
void GreenLedTaskFun(void const * argument)
{
  /* USER CODE BEGIN GreenLedTaskFun */
    /* Infinite loop */
    for(;;)
    {
        vTaskDelay(500);
        HAL_GPIO_TogglePin(GreenLed_GPIO_Port , GreenLed_Pin); 
    }
  /* USER CODE END GreenLedTaskFun */
}

/* USER CODE BEGIN Header_CliTaskFun */
/**
* @brief Function implementing the CliTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CliTaskFun */
void CliTaskFun(void const * argument)
{
  /* USER CODE BEGIN CliTaskFun */
  /* Infinite loop */

    for(;;)
    {
        vTaskDelay(1);
    }
  /* USER CODE END CliTaskFun */
}

/* USER CODE BEGIN Header_GPSTaskFun */
/**
* @brief Function implementing the GPSTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_GPSTaskFun */
void GPSTaskFun(void const * argument)
{
  /* USER CODE BEGIN GPSTaskFun */
  /* Infinite loop */
    uint8 data[500];

    RxUARTGPSStates = NMEAPro_WaitingHeader;
    uartGPS_SelectUartUARTGPSData(UART6);
    HAL_UART_Transmit_IT(&huart6, (uint8_t*) "Activando estructura de recepcion GPS\r", 38);
    HAL_UART_Transmit_IT(&huart4, (uint8_t*) "Activando estructura de recepcion CLI\r", 38);
    osDelay(5);
    /*
    for(;;)
    {
      vTaskDelay(1);
    } 
    */
    GPS_Init(UART6);
    HAL_UART_Receive_IT( &huart4, &tempData, 1);
    
    for (int i=0; i<500; i++)
    {
        data[i] = 0x00;
    }
    
    for(;;)
    {
        if (GPS_ProcessNMEA() == EXIT_SUCCESS)  /* if ( UARTGPSCommandRx == true  )  { */
        { 
            //HAL_GPIO_TogglePin(YellowLed_GPIO_Port , YellowLed_Pin);
            UARTGPSData.u8RxFrameIndex = 0;
            UARTGPSData.u16RxHeadersFrameArray[0] = 0;
            UARTGPSData.u16RxEndsFrameArray[0] = 0;
            UARTGPSData.u16RxBufferIndex = 0;
            GPS_PrintGpsData((char*) data);
            HAL_UART_Transmit_IT((UART_HandleTypeDef*) &huart4, data,  304);
            //HAL_UART_Transmit_IT((UART_HandleTypeDef*) &huart4, (uint8*)"Frame\n",  6);
        }
        osDelay(1);
        //vTaskSuspend(GPSTaskHandle);
    }
  /* USER CODE END GPSTaskFun */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
