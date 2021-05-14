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
#include <stdio.h>
#include <string.h>
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

/* USER CODE BEGIN PV */
char TxDataBuffer[110] =
{ 0 };
char RxDataBuffer[32] =
{ 0 };
char TxMenu[110]=
{0};
char Status[40]=
{0};
char Error[44]=
{0};
uint8_t status=0;
int Time_blink=1;
uint8_t buttonst;
uint8_t state =0;
uint8_t ucount=0;
uint8_t pcount=0;
enum Menu{
  Menu_First_Present=0,
  Menu_First=10,
  Menu_LED_Present=20,
  Menu_LED=30,
  Menu_Button_Present=40,
  Menu_Button=50
}
;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void UARTRecieveAndResponsePolling();
int16_t UARTRecieveIT();
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
  /* USER CODE BEGIN 2 */
  {
  char temp[]="HELLO WORLD\r\n please type something to test UART\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)temp, strlen(temp),10);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		/*Interrupt Mode*/

		HAL_UART_Receive_IT(&huart2,  (uint8_t*)RxDataBuffer, 32);
		/*Method 2 W/ 1 Char Received*/
		int16_t inputchar = UARTRecieveIT();
		switch (state)
		  {
 case Menu_First_Present:
     sprintf(TxDataBuffer,"press0_to_LED\r\npress1_to_Button\r\n---------------------------------------------\r\n");
     HAL_UART_Transmit_IT(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer));
     state = Menu_First;
     break;
   case Menu_First:
   switch (inputchar)
   {
   case '0':
     state = Menu_LED_Present;
     break;
   case '1':
     state = Menu_Button_Present;
     break;
   case -1:
     break;
   default:
	   sprintf(Error,"-Error-\r\n--------------------------------\r\n");
	   	HAL_UART_Transmit_IT(&huart2, (uint8_t*)Error, strlen(Error));
	   	state = Menu_First_Present;
     break;
   }
   break;
    case Menu_LED_Present:
     sprintf(TxMenu,"LED_Control\r\na-increase_frequency\r\ns-decrease_frequency\r\nd-ON/OFF\r\ne-Exit\r\n--------------------------------\r\n");
     HAL_UART_Transmit_IT(&huart2, (uint8_t*)TxMenu, strlen(TxMenu));
     state = Menu_LED;
     break;
   case Menu_LED:
   switch (inputchar)
   {
   case 'a':
	   Time_blink+=1;
	   sprintf(Status,"Frequency=[%d]\r\n",Time_blink);
	   HAL_UART_Transmit_IT(&huart2, (uint8_t*)Status, strlen(Status));
	   break;
   case 's':
	   Time_blink-=1;
	   if(Time_blink<=0){
	   	Time_blink=0;
	   }else{

	   }
	   sprintf(Status,"Frequency=[%d]\r\n",Time_blink);
	   HAL_UART_Transmit_IT(&huart2, (uint8_t*)Status, strlen(Status));
       break;
   case 'd':
	   if(status==0){
		   status=1;
		   sprintf(Status,"LED_ON\r\n",Time_blink);
		   HAL_UART_Transmit_IT(&huart2, (uint8_t*)Status, strlen(Status));
	   }else{
		   status=0;
		   sprintf(Status,"LED_OFF\r\n",Time_blink);
		   HAL_UART_Transmit_IT(&huart2, (uint8_t*)Status, strlen(Status));
	   }
	   break;
   case 'e':
     state = Menu_First_Present;
     break;
   case -1:
	 break;
   default:
	   sprintf(Error,"-Error-\r\n--------------------------------\r\n");
	   HAL_UART_Transmit_IT(&huart2, (uint8_t*)Error, strlen(Error));
	   state = Menu_LED_Present;
     break;
   }
   break;

   case Menu_Button_Present:
     sprintf(TxMenu,"Button_Status\r\ne-exit\r\n--------------------------------\r\n");
     HAL_UART_Transmit_IT(&huart2, (uint8_t*)TxMenu, strlen(TxMenu));
     state = Menu_Button;
     break;
   case Menu_Button:
   switch (inputchar)
   {
   case 'e':
     state = Menu_First_Present;
     break;
   case -1:
	   if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)==GPIO_PIN_RESET && pcount==0){
	  	  sprintf(Status,"button_press\r\n");
	  	  HAL_UART_Transmit_IT(&huart2, (uint8_t*)Status, strlen(Status));
	     	ucount=0;
	     	pcount=1;
	     }else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)==GPIO_PIN_SET && ucount==0){
	     sprintf(Status,"unpress\r\n");
	     HAL_UART_Transmit_IT(&huart2, (uint8_t*)Status, strlen(Status));
	        ucount=1;
	    	pcount=0;
	     }else{
	     }
  	 break;
   default:
	   sprintf(Error,"-Error-\r\n--------------------------------\r\n");
	   HAL_UART_Transmit_IT(&huart2, (uint8_t*)Error, strlen(Error));
     break;
   }
   break;
   }

		/*This section just simmulate Work Load*/
if(Time_blink==0 && status==1){
			HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,GPIO_PIN_SET);
		}else if(status==1){
			HAL_Delay(500/Time_blink);
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		}else if(status==0){
			HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,GPIO_PIN_RESET);
		}
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
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
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
int16_t UARTRecieveIT()
{
	static uint32_t dataPos =0;
	int16_t data=-1;
	if(huart2.RxXferSize - huart2.RxXferCount!=dataPos)
	{
		data=RxDataBuffer[dataPos];
		dataPos= (dataPos+1)%huart2.RxXferSize;
	}
	return data;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	sprintf(TxDataBuffer, "Received:[%s]\r\n", RxDataBuffer);
	//HAL_UART_Transmit(&huart2, (uint8_t*)TxDataBuffer, strlen(TxDataBuffer), 1000);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
