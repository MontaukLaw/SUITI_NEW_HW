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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BL_RTX_Pin GPIO_PIN_14
#define BL_RTX_GPIO_Port GPIOC
#define CHRG_Pin GPIO_PIN_15
#define CHRG_GPIO_Port GPIOC
#define OUT0_Pin GPIO_PIN_0
#define OUT0_GPIO_Port GPIOC
#define OUT1_Pin GPIO_PIN_1
#define OUT1_GPIO_Port GPIOC
#define OUT2_Pin GPIO_PIN_2
#define OUT2_GPIO_Port GPIOC
#define OUT3_Pin GPIO_PIN_3
#define OUT3_GPIO_Port GPIOC
#define OUT15_Pin GPIO_PIN_1
#define OUT15_GPIO_Port GPIOA
#define OUT14_Pin GPIO_PIN_2
#define OUT14_GPIO_Port GPIOA
#define OUT13_Pin GPIO_PIN_3
#define OUT13_GPIO_Port GPIOA
#define OUT12_Pin GPIO_PIN_4
#define OUT12_GPIO_Port GPIOA
#define OUT11_Pin GPIO_PIN_5
#define OUT11_GPIO_Port GPIOA
#define OUT10_Pin GPIO_PIN_6
#define OUT10_GPIO_Port GPIOA
#define OUT9_Pin GPIO_PIN_7
#define OUT9_GPIO_Port GPIOA
#define OUT8_Pin GPIO_PIN_4
#define OUT8_GPIO_Port GPIOC
#define OUT7_Pin GPIO_PIN_0
#define OUT7_GPIO_Port GPIOB
#define OUT6_Pin GPIO_PIN_1
#define OUT6_GPIO_Port GPIOB
#define OUT5_Pin GPIO_PIN_2
#define OUT5_GPIO_Port GPIOB
#define OUT4_Pin GPIO_PIN_10
#define OUT4_GPIO_Port GPIOB
#define FLASH_CS_Pin GPIO_PIN_12
#define FLASH_CS_GPIO_Port GPIOB
#define SW_4067_S3_Pin GPIO_PIN_6
#define SW_4067_S3_GPIO_Port GPIOC
#define SW_4067_S2_Pin GPIO_PIN_7
#define SW_4067_S2_GPIO_Port GPIOC
#define SW_4067_S1_Pin GPIO_PIN_8
#define SW_4067_S1_GPIO_Port GPIOC
#define SW_4067_S0_Pin GPIO_PIN_9
#define SW_4067_S0_GPIO_Port GPIOC
#define LED_G_Pin GPIO_PIN_15
#define LED_G_GPIO_Port GPIOA
#define LED_R_Pin GPIO_PIN_10
#define LED_R_GPIO_Port GPIOC
#define PWR_CTRL_Pin GPIO_PIN_12
#define PWR_CTRL_GPIO_Port GPIOC
#define KEY_Pin GPIO_PIN_2
#define KEY_GPIO_Port GPIOD
#define LED_B_Pin GPIO_PIN_5
#define LED_B_GPIO_Port GPIOB
#define BL_LINK_Pin GPIO_PIN_7
#define BL_LINK_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
