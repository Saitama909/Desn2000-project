/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum {
	TIMER_MODE,
	CLOCK_MODE
} MainMode;

typedef enum {
	TIMER1,
	TIMER2,
	TIMER3,
	TIMER4
} TimerMode;

typedef enum {
	CLOCK,
	ALARM,
	COUNTDOWN,
	STOPWATCH
} ClockMode;

typedef enum {
	DISPLAY,
	CONFIG
} ModeState;

typedef struct {
	MainMode mainMode;
	TimerMode timerMode;
	ClockMode clockMode;
	ModeState modeState;
} DeviceState;

extern volatile DeviceState deviceState;
extern volatile DeviceState prevState;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim16;
extern RTC_HandleTypeDef hrtc;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern void toggleStopwatch(void);
extern void resetStopwatch(void);
extern bool hasStateChanged(DeviceState currentState);
extern void Motor(int steps);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define SRCLK_Pin GPIO_PIN_0
#define SRCLK_GPIO_Port GPIOC
#define SW1_Pin GPIO_PIN_1
#define SW1_GPIO_Port GPIOA
#define SW1_EXTI_IRQn EXTI1_IRQn
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define SW2_Pin GPIO_PIN_4
#define SW2_GPIO_Port GPIOA
#define SW2_EXTI_IRQn EXTI4_IRQn
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define COIL_A_Pin GPIO_PIN_5
#define COIL_A_GPIO_Port GPIOC
#define SW3_Pin GPIO_PIN_0
#define SW3_GPIO_Port GPIOB
#define SW3_EXTI_IRQn EXTI0_IRQn
#define COIL_D_Pin GPIO_PIN_2
#define COIL_D_GPIO_Port GPIOB
#define LED_D1_Pin GPIO_PIN_10
#define LED_D1_GPIO_Port GPIOB
#define SER_Pin GPIO_PIN_15
#define SER_GPIO_Port GPIOB
#define COIL_C_Pin GPIO_PIN_7
#define COIL_C_GPIO_Port GPIOC
#define COIL_B_Pin GPIO_PIN_12
#define COIL_B_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define LED_D2_Pin GPIO_PIN_4
#define LED_D2_GPIO_Port GPIOB
#define LED_D3_Pin GPIO_PIN_5
#define LED_D3_GPIO_Port GPIOB
#define RCLK_Pin GPIO_PIN_7
#define RCLK_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define SW4_Pin GPIO_PIN_1
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif