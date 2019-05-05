/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
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
#define LD2_Pin GPIO_PIN_13
#define LD2_GPIO_Port GPIOC
#define RCLK_Pin GPIO_PIN_1
#define RCLK_GPIO_Port GPIOA
#define RCLK_EXTI_IRQn EXTI1_IRQn
#define RDT_Pin GPIO_PIN_2
#define RDT_GPIO_Port GPIOA
#define RDT_EXTI_IRQn EXTI2_IRQn
#define RSW_Pin GPIO_PIN_3
#define RSW_GPIO_Port GPIOA
#define RSW_EXTI_IRQn EXTI3_IRQn
#define RCLK2_Pin GPIO_PIN_4
#define RCLK2_GPIO_Port GPIOA
#define RCLK2_EXTI_IRQn EXTI4_IRQn
#define RDT2_Pin GPIO_PIN_5
#define RDT2_GPIO_Port GPIOA
#define RDT2_EXTI_IRQn EXTI9_5_IRQn
#define RSW2_Pin GPIO_PIN_6
#define RSW2_GPIO_Port GPIOA
#define RSW2_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
