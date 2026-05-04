/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
/**
 * Development of Instrumentation for Grab Sampling in 100k Tank
 * Use a USB port for USART2 communications. Use a serial PuTTY terminal on select COM# port with 115200 baud
 * Do not use HAL_DELAY inside the while inside keystroke timer logic, Sampling script should exist outside of this timer.
 * Many of the Current User Functions for Motion Aren't currently implemented in the While() core of the Program
 */
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
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

UART_HandleTypeDef huart2;

uint8_t g_key_data = 0;      // Stores the received character
uint8_t g_new_key_flag = 0;

uint8_t tx_buffer[27] = "GRAB SAMPLER READY\n\r";
uint8_t tx_return[4] = "\n\r";
uint8_t rx_indx;
uint8_t rx_data[1];
uint8_t rx_buffer[100];
uint8_t transfer_cplt;
/*
 * Duty: 0 -> STM32 Hex: {0x80, 0x20, 0x00, 0x00, 0x5B, 0xFE}
Duty: 4000 -> STM32 Hex: {0x80, 0x20, 0x0F, 0xA0, 0xFE, 0x2A}
Duty: 8000 -> STM32 Hex: {0x80, 0x20, 0x1F, 0x40, 0x00, 0x77}
Duty: 10000 -> STM32 Hex: {0x80, 0x20, 0x27, 0x10, 0xD6, 0xBE}
Duty: 20000 -> STM32 Hex: {0x80, 0x20, 0x4E, 0x20, 0x51, 0x5F}
Duty: 25000 -> STM32 Hex: {0x80, 0x20, 0x61, 0xA8, 0x57, 0x07}
Duty: 30000 -> STM32 Hex: {0x80, 0x20, 0x75, 0x30, 0x9A, 0x01}
Duty: 32000 -> STM32 Hex: {0x80, 0x20, 0x7D, 0x00, 0x25, 0xFB}
Duty: 30000 -> STM32 Hex: {0x80, 0x20, 0x75, 0x30, 0x9A, 0x01}
Duty: 25000 -> STM32 Hex: {0x80, 0x20, 0x61, 0xA8, 0x57, 0x07}
Duty: 20000 -> STM32 Hex: {0x80, 0x20, 0x4E, 0x20, 0x51, 0x5F}
Duty: 10000 -> STM32 Hex: {0x80, 0x20, 0x27, 0x10, 0xD6, 0xBE}
Duty: 8000 -> STM32 Hex: {0x80, 0x20, 0x1F, 0x40, 0x00, 0x77}
Duty: 4000 -> STM32 Hex: {0x80, 0x20, 0x0F, 0xA0, 0xFE, 0x2A}
Duty: 0 -> STM32 Hex: {0x80, 0x20, 0x00, 0x00, 0x5B, 0xFE}

 */


const uint8_t RC_ACCEL_FULLFORWARD[] = {0x80, 0x34, 0x75, 0x30, 0x00, 0x00, 0x17, 0x70, 0x8B, 0xCA};//Acceleration Commands ripped from Arduino Libraries
const uint8_t RC_ACCEL_FULLREVERSE[] = {0x80, 0x34, 0x8A, 0xD0, 0x00, 0x00, 0x17, 0x70, 0xE4, 0x79};
const uint8_t RC_ACCEL_STOP[] = {0x80, 0x34, 0x00, 0x00, 0x00, 0x00, 0x17, 0x70, 0x81, 0xB9};

//uint8_t sampling_halt_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

uint8_t MAX_Register_Read(uint8_t reg)//For USB Host Shield When Implemented
{
	uint8_t address = (reg << 3) | 0x00;//Address
	uint8_t data = 0;//RD/WR
	uint8_t dummy = 0x00;
	//CS to activate
	HAL_GPIO_WritePin(USB_CS_GPIO_Port, USB_CS_Pin, GPIO_PIN_RESET);
	//for(volatile int i=0; i<100; i++);
	//Send Register Request 8bits
	HAL_SPI_Transmit(&hspi1,&address,1,10);
	//REceieve Reg COntent 8bits
	HAL_SPI_TransmitReceive(&hspi1, &dummy, &data, 1, 10);

	HAL_SPI_Transmit(&hspi1,&address,1,10);
		//REceieve Reg COntent 8bits
		HAL_SPI_TransmitReceive(&hspi1, &dummy, &data, 1, 10);
	//Kill CS to end
	//for(volatile int i=0; i<100; i++);
	HAL_GPIO_WritePin(USB_CS_GPIO_Port, USB_CS_Pin, GPIO_PIN_SET);
	return data;
}

void Screw_Forward(void){}
void Screw_Backward(void){}
void Screw_Pivot_Left(void){}
void Screw_Pivot_Right(void){}
void Screw_Stop(void){}

void Change_Barrell(void){}
void Drill_Forward(void){}
void Drill_Reverse(void){}
void Drill_Stop(void){}

void Actuator1_Forward(void)
{
	if(HAL_GPIO_ReadPin(Actuator1_DIR_GPIO_Port,Actuator1_DIR_Pin) == GPIO_PIN_RESET){
		return;
		}
		else{
			HAL_GPIO_WritePin(Actuator1_PWR_GPIO_Port,    Actuator1_PWR_Pin,    GPIO_PIN_RESET);     // Power off
			HAL_Delay(5);
			HAL_GPIO_WritePin(Actuator1_DIR_GPIO_Port, Actuator1_DIR_Pin, GPIO_PIN_RESET);  // Forward direction
			HAL_Delay(5);
			HAL_GPIO_WritePin(Actuator1_PWR_GPIO_Port,    Actuator1_PWR_Pin,    GPIO_PIN_SET);     // Power on
		}
}

/* Turn on Actuator 1 Reverse */
void Actuator1_Reverse(void)
{
	if(HAL_GPIO_ReadPin(Actuator1_DIR_GPIO_Port,Actuator1_DIR_Pin) == GPIO_PIN_SET){
		return;
	}
	else{
	    HAL_GPIO_WritePin(Actuator1_PWR_GPIO_Port,    Actuator1_PWR_Pin,    GPIO_PIN_RESET);    // Power off
	    HAL_Delay(5);
	    HAL_GPIO_WritePin(Actuator1_DIR_GPIO_Port, Actuator1_DIR_Pin, GPIO_PIN_SET);    // Reverse direction
	    HAL_Delay(5);
	    HAL_GPIO_WritePin(Actuator1_PWR_GPIO_Port,    Actuator1_PWR_Pin,    GPIO_PIN_SET);    // Power on
	}
}

/* Turn off Actuator 1 */
void Actuator1_Stop(void)
{
    HAL_GPIO_WritePin(Actuator1_PWR_GPIO_Port, Actuator1_PWR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Actuator1_DIR_GPIO_Port, Actuator1_DIR_Pin, GPIO_PIN_RESET);// Power off
}

/* Turn on Actuator 2 Forward */
void Actuator2_Forward(void)
{
	if(HAL_GPIO_ReadPin(Actuator2_DIR_GPIO_Port,Actuator2_DIR_Pin) == GPIO_PIN_RESET){
		return;
	}
	else{
		HAL_GPIO_WritePin(Actuator2_PWR_GPIO_Port,    Actuator2_PWR_Pin,    GPIO_PIN_RESET);     // Power off
		HAL_Delay(5);
		HAL_GPIO_WritePin(Actuator2_DIR_GPIO_Port, Actuator2_DIR_Pin, GPIO_PIN_RESET);  // Forward direction
		HAL_Delay(5);
		HAL_GPIO_WritePin(Actuator2_PWR_GPIO_Port,    Actuator2_PWR_Pin,    GPIO_PIN_SET);     // Power on
	}
}

/* Turn on Actuator 2 Reverse */
void Actuator2_Reverse(void)
{
	if(HAL_GPIO_ReadPin(Actuator2_DIR_GPIO_Port,Actuator2_DIR_Pin) == GPIO_PIN_SET){
		return;
	}
	else{
		HAL_GPIO_WritePin(Actuator2_PWR_GPIO_Port, Actuator2_PWR_Pin, GPIO_PIN_RESET);    // Power off
		HAL_Delay(5);
		HAL_GPIO_WritePin(Actuator2_DIR_GPIO_Port, Actuator2_DIR_Pin, GPIO_PIN_SET);    // Reverse direction
		HAL_Delay(5);
		HAL_GPIO_WritePin(Actuator2_PWR_GPIO_Port,    Actuator2_PWR_Pin,    GPIO_PIN_SET);    // Power on
	}
}

/* Turn off Actuator 2 */
void Actuator2_Stop(void)
{
    HAL_GPIO_WritePin(Actuator2_PWR_GPIO_Port, Actuator2_PWR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Actuator2_DIR_GPIO_Port, Actuator2_DIR_Pin, GPIO_PIN_RESET);// Power off
}
/* Turn on Trigger Controlling Actuator in reverse */
void Trigger_Actuator_Reverse(void)
{
	if(HAL_GPIO_ReadPin(Trigger_Actuator_DIR_GPIO_Port,Trigger_Actuator_DIR_Pin) == GPIO_PIN_RESET){
		return;
	}
	else{
		HAL_GPIO_WritePin(Trigger_Actuator_PWR_GPIO_Port,    Trigger_Actuator_PWR_Pin,    GPIO_PIN_RESET);     // Power off
		HAL_Delay(5);
		HAL_GPIO_WritePin(Trigger_Actuator_DIR_GPIO_Port, Trigger_Actuator_DIR_Pin, GPIO_PIN_RESET);  // Forward direction
		HAL_Delay(5);
		HAL_GPIO_WritePin(Trigger_Actuator_PWR_GPIO_Port,    Trigger_Actuator_PWR_Pin,    GPIO_PIN_SET);     // Power on
	}
}

/* Turn on Trigger controller forward */
void Trigger_Actuator_Forward(void)
{
	if(HAL_GPIO_ReadPin(Trigger_Actuator_DIR_GPIO_Port,Trigger_Actuator_DIR_Pin) == GPIO_PIN_SET){
		return;
	}
	else{
		HAL_GPIO_WritePin(Trigger_Actuator_PWR_GPIO_Port, Trigger_Actuator_PWR_Pin, GPIO_PIN_RESET);    // Power off
		HAL_Delay(5);
		HAL_GPIO_WritePin(Trigger_Actuator_DIR_GPIO_Port, Trigger_Actuator_DIR_Pin, GPIO_PIN_SET);    // Reverse direction
		HAL_Delay(5);
		HAL_GPIO_WritePin(Trigger_Actuator_PWR_GPIO_Port,    Trigger_Actuator_PWR_Pin,    GPIO_PIN_SET);    // Power on
	}
}

/* Turn off Trigger Controlling Actuator */
void Trigger_Actuator_Stop(void)
{
    HAL_GPIO_WritePin(Trigger_Actuator_PWR_GPIO_Port, Trigger_Actuator_PWR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Trigger_Actuator_DIR_GPIO_Port, Trigger_Actuator_DIR_Pin, GPIO_PIN_RESET);// Power off
}
void M1_A(void)
{
    HAL_GPIO_WritePin(M1_A_GPIO_Port, M1_A_Pin, GPIO_PIN_SET);
}
// Function to set roller motor speed as a percentage
void M1_Fwd_Set_Speed(float Full_Speed_Percent){

	//uint8_t periodSet = Full_Speed_Percent;

}
void Sampling_Script(void)
{
	/*
	 * Rotate barrel to empty sample
	 * Push drill, rotate to engage lock
	 * Pull Drill and Sample bit out of barrel changer
	 * Maybe re adjust the Barrel
	 * Raise sampling platform
	 * Correct drill direction and start drill
	 * Push drill into ground while Drilling to cut sample
	 * Stop Drill
	 * Pull drill out of ground
	 * lower platform
	 * Adjust Barrel
	 * push sample into barrel
	 * Reverse drill direction with short rotation
	 * Pull drill a little
	 *
	 * -----End of process.
	 */
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
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  uint32_t lskeytime = 0;//Keystroke Timer Variables
  const uint32_t timeout = 250;//Refresh time ms
  char input = 0;

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);// Initialize/Default spot for PWM servo
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1500);
  uint32_t servoPos = 1495;

  HAL_UART_Transmit(&huart2, tx_buffer, 27, 10);//Welcome Message send
  Actuator1_Stop();//Put initial states for pins here if not initialized
  Actuator2_Stop();
  Trigger_Actuator_Stop();





  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1){
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (HAL_GPIO_ReadPin(Limit_Switch_GPIO_Port, Limit_Switch_Pin) == GPIO_PIN_SET) {//Stops Sampling Arm at Horizontal
			  if (input != 'q' && input != 'Q'){//Prevents Locking While Horizontal
	  	       Actuator1_Stop();
	          }
	  }
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, servoPos);
/*	if (sampling_halt_flag[0] == 1)// Proto script for a sampling process
	{
		  Stop_All();
		  Hal_Delay(100);
		  Sampling_Script();
		  HAL_Delay(100);
		  sampling_halt_flag[0] = 0;
	}  */
	if (HAL_UART_Receive(&huart2, rx_data,1,10) == HAL_OK){
		input = (char)rx_data[0];
		lskeytime = HAL_GetTick();
		// For Demonstration HAL_UART_Transmit(&huart2, rx_data, 1, 10); // Returns Current Key to Port for debugging

		if (input == 'q' || input == 'Q'){// Extend Actuator 1
			HAL_GPIO_WritePin(Actuator1_PWR_GPIO_Port, Actuator1_PWR_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Actuator1_DIR_GPIO_Port, Actuator1_DIR_Pin, GPIO_PIN_RESET);
		}
		if (input == 'w' || input == 'W'){// Extend Actuator 2
			HAL_GPIO_WritePin(Actuator2_PWR_GPIO_Port, Actuator2_PWR_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Actuator2_DIR_GPIO_Port, Actuator2_DIR_Pin, GPIO_PIN_RESET);
		}
		if (input == 'e' || input == 'E'){// Small Servo Limits at around 310 and 2670
			//__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, servoPos);// Move Servo to Position
			if (servoPos <= 2670){
				servoPos = servoPos + 20;
			}
		}
		if (input == 'r' || input == 'R'){// Trigger Actuator
			HAL_GPIO_WritePin(Trigger_Actuator_PWR_GPIO_Port, Trigger_Actuator_PWR_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Trigger_Actuator_DIR_GPIO_Port, Trigger_Actuator_DIR_Pin, GPIO_PIN_SET);
		}
		if (input == 't' || input == 'T'){// Extend Actuator 2

		}
		if (input == 'a' || input == 'A'){// Retract Actuator 1

			if (HAL_GPIO_ReadPin(Limit_Switch_GPIO_Port, Limit_Switch_Pin) == GPIO_PIN_RESET) {// Unless Limit Is Hit
				HAL_GPIO_WritePin(Actuator1_PWR_GPIO_Port, Actuator1_PWR_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(Actuator1_DIR_GPIO_Port, Actuator1_DIR_Pin, GPIO_PIN_SET);

			}
		}
		if (input == 's' || input == 'S'){// Retract Actuator 2
			HAL_GPIO_WritePin(Actuator2_PWR_GPIO_Port, Actuator2_PWR_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Actuator2_DIR_GPIO_Port, Actuator2_DIR_Pin, GPIO_PIN_SET);
		}
		if (input == 'd' || input == 'D'){//Direction Servo Controls, Use servoPos to gradually change and COMPARE to set position
			//__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, servoPos);
			if (servoPos >= 310){
			servoPos = servoPos - 20;
			}
		}
		if (input == 'f' || input == 'F'){// Extend Drill Trigger Actuator
			HAL_GPIO_WritePin(Trigger_Actuator_DIR_GPIO_Port, Trigger_Actuator_DIR_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Trigger_Actuator_PWR_GPIO_Port, Trigger_Actuator_PWR_Pin, GPIO_PIN_SET);
		}
		if (input == 'g' || input == 'G'){// Extend Actuator 2

		}
		if (input == 'i' || input == 'I'){// Left Motor Control
					HAL_UART_Transmit(&huart5, (uint8_t*)RC_ACCEL_FULLFORWARD, 10, 10);  // Forward direction
				}
		if (input == 'k' || input == 'K'){// Left Motor Control
					HAL_UART_Transmit(&huart5, (uint8_t*)RC_ACCEL_FULLREVERSE, 10, 10);  // Forward direction
				}
		if (input == 'y' || input == 'Y'){//

		}
		if (input == 'h' || input == 'H'){//

		}
		if (input == ' '){// MISC Reverse
			Actuator1_Stop();
			Actuator2_Stop();
			Trigger_Actuator_Stop();

		}
	}
	if (input != 0 && (HAL_GetTick() - lskeytime > timeout)){
/* For Demonstration
		Actuator1_Stop();
		Actuator2_Stop();
		Trigger_Actuator_Stop();
*/
		HAL_UART_Transmit(&huart5, (uint8_t*)RC_ACCEL_STOP, 10, 10);
		input = 0;

	}
	 //Control and Comms Logic

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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 169;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 3029;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 169;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 3030;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart5, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart5, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Trigger_Actuator_PWR_Pin|Trigger_Actuator_DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, USB_CS_Pin|Actuator1_PWR_Pin|Actuator2_DIR_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_IRQ_GPIO_Port, USB_IRQ_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Actuator1_DIR_GPIO_Port, Actuator1_DIR_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Actuator2_PWR_GPIO_Port, Actuator2_PWR_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : Trigger_Actuator_PWR_Pin Trigger_Actuator_DIR_Pin Actuator1_DIR_Pin */
  GPIO_InitStruct.Pin = Trigger_Actuator_PWR_Pin|Trigger_Actuator_DIR_Pin|Actuator1_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_CS_Pin Actuator1_PWR_Pin Actuator2_DIR_Pin */
  GPIO_InitStruct.Pin = USB_CS_Pin|Actuator1_PWR_Pin|Actuator2_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_IRQ_Pin Actuator2_PWR_Pin */
  GPIO_InitStruct.Pin = USB_IRQ_Pin|Actuator2_PWR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : Limit_Switch_Pin */
  GPIO_InitStruct.Pin = Limit_Switch_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(Limit_Switch_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/*
int _write(int file, char *ptr, int len)
{
  (void)file;
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    ITM_SendChar(*ptr++);
  }
  return len;
}*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//Controls
{
    UNUSED(huart);

    HAL_UART_Transmit(&huart2, rx_data, 1, 10);
}

// Redirects printf to the Virtual COM Port (USART2)
/*int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, 100);
  return len;
}*/
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
#ifdef USE_FULL_ASSERT
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
