/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

#include "stm32g4xx_nucleo.h"
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RCC_OSC32_IN_Pin GPIO_PIN_14
#define RCC_OSC32_IN_GPIO_Port GPIOC
#define RCC_OSC32_OUT_Pin GPIO_PIN_15
#define RCC_OSC32_OUT_GPIO_Port GPIOC
#define RCC_OSC_IN_Pin GPIO_PIN_0
#define RCC_OSC_IN_GPIO_Port GPIOF
#define RCC_OSC_OUT_Pin GPIO_PIN_1
#define RCC_OSC_OUT_GPIO_Port GPIOF
#define M1_A_Pin GPIO_PIN_0
#define M1_A_GPIO_Port GPIOC
#define M1_B_Pin GPIO_PIN_1
#define M1_B_GPIO_Port GPIOC
#define Servo1_PWM_Pin GPIO_PIN_0
#define Servo1_PWM_GPIO_Port GPIOA
#define Servo2_PWM_Pin GPIO_PIN_1
#define Servo2_PWM_GPIO_Port GPIOA
#define Actuator2_PWR_Pin GPIO_PIN_6
#define Actuator2_PWR_GPIO_Port GPIOC
#define Actuator2_FwdRev_Pin GPIO_PIN_12
#define Actuator2_FwdRev_GPIO_Port GPIOA
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define Actuator1_PWR_Pin GPIO_PIN_15
#define Actuator1_PWR_GPIO_Port GPIOA
#define Motor1_Tx_Pin GPIO_PIN_10
#define Motor1_Tx_GPIO_Port GPIOC
#define Motor1_Rx_Pin GPIO_PIN_11
#define Motor1_Rx_GPIO_Port GPIOC
#define Motor2_Tx_Pin GPIO_PIN_12
#define Motor2_Tx_GPIO_Port GPIOC
#define Motor2_Rx_Pin GPIO_PIN_2
#define Motor2_Rx_GPIO_Port GPIOD
#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB
#define Actuator1_FwdRev_Pin GPIO_PIN_8
#define Actuator1_FwdRev_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
