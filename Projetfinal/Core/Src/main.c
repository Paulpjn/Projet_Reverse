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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define RX_BUFFER_SIZE 64
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_data;
uint8_t rx_index = 0;
uint8_t chenillard_active = 0;
uint8_t etat = 1;
TIM_HandleTypeDef *current_timer = &htim2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
void ProcessCommand(uint8_t *command);
void SendResponse(char *response);
void chenillard1(void);
void change_timer(uint8_t timer);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void chenillard1(void) {
  static uint8_t step = 0;
  // Éteint toutes les LEDs
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14, GPIO_PIN_RESET);
  // Allume la LED correspondante
  switch(step) {
      case 0: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); break;
      case 1: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); break;
      case 2: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); break;
      case 3: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); break;
  }
  step = (step + 1) % 4;
}
void chenillard2(void) {
  static uint8_t step = 0;
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14, GPIO_PIN_RESET);
  if (step == 0) {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_14, GPIO_PIN_SET);
  } else {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  }
  step = (step + 1) % 2;
}
void chenillard3(void) {
  static uint8_t step = 0;
  if (step == 0) {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14, GPIO_PIN_SET);
  } else {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14, GPIO_PIN_RESET);
  }
  step = (step + 1) % 2;
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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

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
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
HAL_UART_Receive_IT(&huart3, &rx_data, 1);
// Démarrer seulement le timer 2 initialement
HAL_TIM_Base_Start_IT(&htim2);
HAL_TIM_Base_Start_IT(&htim3);
HAL_TIM_Base_Start_IT(&htim4);
current_timer = &htim2;
  /* USER CODE END 2 */

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

/* USER CODE BEGIN 4 */
// Callback de réception UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
   if (huart->Instance == USART3) {

       if (rx_data == '\0') {
           if (rx_index > 0) {
               if (strcmp((char*)rx_buffer,"LED1 ON") ==0){
            	   if (etat == 1){
            	   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, SET);}
               	   }
               else if (strcmp((char*)rx_buffer,"LED1 OFF") ==0){
            	   if (etat == 1){
				   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, RESET);}
					   }
               else if (strcmp((char*)rx_buffer,"LED2 ON") ==0){
            	   if (etat == 1){
				   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, SET);}
					   }
               else if (strcmp((char*)rx_buffer,"LED2 OFF") ==0){
            	   if (etat == 1){
				   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, RESET);}
					   }
               else if (strcmp((char*)rx_buffer,"LED3 ON") ==0){
            	   if (etat == 1){
				   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, SET);}
					   }
               else if (strcmp((char*)rx_buffer,"LED3 OFF") ==0){
            	   if (etat == 1){
				   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, RESET);}
					   }
               else if (strcmp((char*)rx_buffer,"LED3 OFF") ==0){
            	   if (etat == 1){
               	   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, RESET);}
               					   }
               else if (strcmp((char*)rx_buffer,"CHENILLARD1 ON") ==0){
							   HAL_TIM_Base_Start_IT(current_timer);
							   chenillard_active = 1;
							   etat = 0;
								   }
               else if (strcmp((char*)rx_buffer,"CHENILLARD1 OFF") ==0){
            	   	if (chenillard_active ==1){
							   HAL_TIM_Base_Stop(current_timer);
							   etat = 1;
							   TurnOffAllLEDs();}
								   }
               else if (strcmp((char*)rx_buffer,"CHENILLARD2 ON") ==0){
							   HAL_TIM_Base_Start_IT(current_timer);
							   chenillard_active = 2;
							   etat = 0;
								   }
			   else if (strcmp((char*)rx_buffer,"CHENILLARD2 OFF") ==0){
				   if (chenillard_active == 2){
							   HAL_TIM_Base_Stop(current_timer);
							   etat = 1;
							   TurnOffAllLEDs();}
								   }
			   else if (strcmp((char*)rx_buffer,"CHENILLARD3 ON") ==0){
							   HAL_TIM_Base_Start_IT(current_timer);
							   chenillard_active = 3;
							   etat = 0;
								   }
			   else if (strcmp((char*)rx_buffer,"CHENILLARD3 OFF") ==0){
				   if (chenillard_active == 3){
							   HAL_TIM_Base_Stop(current_timer);
							   etat = 1;
							   TurnOffAllLEDs();}
								   }
			   else if (strcmp((char*)rx_buffer,"FREQUENCE1") ==0){
			                   current_timer = &htim2;
			                  		    }
			   else if (strcmp((char*)rx_buffer,"FREQUENCE2") ==0){
							   current_timer = &htim3;
									    }
			   else if (strcmp((char*)rx_buffer,"FREQUENCE3") ==0){
							   current_timer = &htim4;
									    }
               memset(rx_buffer,0,sizeof(rx_buffer));
               rx_index = 0;
           }
       }
       else if (rx_index < RX_BUFFER_SIZE - 1) {
           rx_buffer[rx_index] = rx_data;
           rx_index++;
       }
       HAL_UART_Receive_IT(&huart3, &rx_data, 1);
   }
}
// Éteint toutes les LEDs
void TurnOffAllLEDs(void) {
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14, GPIO_PIN_RESET);
}
// Callback d'interruption Timer
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
   if (chenillard_active == 0) return;
   if (htim == current_timer) {
       switch(chenillard_active) {
           case 1: chenillard1(); break;
           case 2: chenillard2(); break;
           case 3: chenillard3(); break;
       }
   }
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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
