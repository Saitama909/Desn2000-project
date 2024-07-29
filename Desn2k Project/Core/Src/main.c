/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_keypad.h"
#include "timer_config.h"
#include "stopwatch.h"
#include "clock.h"

#include "stdio.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBOUNCE_DELAY_MS 50  // Debounce delay in milliseconds
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
volatile DeviceState deviceState = {0};
volatile DeviceState previousState = {0};

int inMode = 0;
int reload = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
void CheckDeviceState();
void Motor(int steps);
bool hasStateChanged(DeviceState currentState);

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
  MX_TIM6_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_GPIO_WritePin(RW_PORT, LCD_RW_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(E_PORT, LCD_E_PIN, GPIO_PIN_RESET);

  init_magic();
  shiftByte(0);
  latchData();
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_SendString("Timer Mode:Tim1");
  LCD_SetCursor(1, 0);
  LCD_SendString("DISPLAY");
  // welcome();

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (hasStateChanged(deviceState) || reload) {
		  if (reload) {
			  reload = 0;
		  }
		  HAL_GPIO_WritePin(GPIOA, LD2_Pin, RESET);
		  LCD_Clear();
		  LCD_SetCursor(0, 0);
		  CheckDeviceState();
	  }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x18;
  sTime.Minutes = 0x16;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
  sDate.Month = RTC_MONTH_JULY;
  sDate.Date = 0x27;
  sDate.Year = 0x24;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 71;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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
  huart2.Init.BaudRate = 38400;
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
  __HAL_RCC_GPIOD_CLK_ENABLE();
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SRCLK_Pin|COIL_A_Pin|COIL_C_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|COIL_B_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, COIL_D_Pin|LED_D1_Pin|SER_Pin|LED_D2_Pin
                          |LED_D3_Pin|RCLK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SRCLK_Pin COIL_A_Pin COIL_C_Pin */
  GPIO_InitStruct.Pin = SRCLK_Pin|COIL_A_Pin|COIL_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : SW1_Pin SW2_Pin */
  GPIO_InitStruct.Pin = SW1_Pin|SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin COIL_B_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|COIL_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SW3_Pin */
  GPIO_InitStruct.Pin = SW3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : COIL_D_Pin LED_D1_Pin SER_Pin LED_D2_Pin
                           LED_D3_Pin RCLK_Pin */
  GPIO_InitStruct.Pin = COIL_D_Pin|LED_D1_Pin|SER_Pin|LED_D2_Pin
                          |LED_D3_Pin|RCLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
  GPIO_InitStruct.Pin = LCD_D4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(D_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_D5_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(D_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_D6_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(D_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_D7_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(D_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_E_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(E_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_RW_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RW_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_RS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RS_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = C1_PIN | C2_PIN | C3_PIN |C4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(C_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = R1_PIN | R2_PIN | R3_PIN| R4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(R_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


volatile uint32_t last_interrupt_time_sw1 = 0;
volatile uint32_t last_interrupt_time_sw2 = 0;
volatile uint32_t last_interrupt_time_sw3 = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    uint32_t current_time = HAL_GetTick();

    if (GPIO_Pin == SW1_Pin) {
        if (current_time - last_interrupt_time_sw1 >= DEBOUNCE_DELAY_MS) {
            last_interrupt_time_sw1 = current_time;
            deviceState.mainMode = !deviceState.mainMode;
        }
    } else if (GPIO_Pin == SW2_Pin) {
        if (current_time - last_interrupt_time_sw2 >= DEBOUNCE_DELAY_MS) {
            last_interrupt_time_sw2 = current_time;
            if (deviceState.mainMode == TIMER_MODE && deviceState.timerMode != TIMER4) {
                deviceState.timerMode = deviceState.timerMode + 1;
            } else if (deviceState.mainMode == TIMER_MODE) {
                deviceState.timerMode = TIMER1;
            } else if (deviceState.mainMode == CLOCK_MODE && deviceState.clockMode != STOPWATCH) {
                deviceState.clockMode = deviceState.clockMode + 1;
            } else {
                deviceState.clockMode = CLOCK;
            }
        }
    } else if (GPIO_Pin == SW3_Pin) {
        if (current_time - last_interrupt_time_sw3 >= DEBOUNCE_DELAY_MS) {
            last_interrupt_time_sw3 = current_time;
            deviceState.modeState = !deviceState.modeState;
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	milliseconds++;
}

bool hasStateChanged(DeviceState currentState) {
    bool changed = false;

    if (currentState.mainMode != previousState.mainMode ||
        currentState.timerMode != previousState.timerMode ||
        currentState.clockMode != previousState.clockMode ||
        currentState.modeState != previousState.modeState) {
        changed = true;
    }

    // Update previousState to the current state
    previousState = currentState;

    return changed;
}

void Motor(int steps) {
    int step = 0;
    for (int i = 0; i < steps; i++) {
    	switch(step) {
			case 0:
				HAL_GPIO_WritePin(COIL_B_GPIO_Port, COIL_B_Pin, 0);
				HAL_GPIO_WritePin(COIL_D_GPIO_Port, COIL_D_Pin, 0);
				HAL_GPIO_WritePin(COIL_A_GPIO_Port, COIL_A_Pin, 0);
				HAL_GPIO_WritePin(COIL_C_GPIO_Port, COIL_C_Pin, 1);
				break;
			case 1:
				HAL_GPIO_WritePin(COIL_B_GPIO_Port, COIL_B_Pin, 0);
				HAL_GPIO_WritePin(COIL_D_GPIO_Port, COIL_D_Pin, 1);
				HAL_GPIO_WritePin(COIL_A_GPIO_Port, COIL_A_Pin, 0);
				HAL_GPIO_WritePin(COIL_C_GPIO_Port, COIL_C_Pin, 1);
				break;
			case 2:
				HAL_GPIO_WritePin(COIL_B_GPIO_Port, COIL_B_Pin, 0);
				HAL_GPIO_WritePin(COIL_D_GPIO_Port, COIL_D_Pin, 1);
				HAL_GPIO_WritePin(COIL_A_GPIO_Port, COIL_A_Pin, 0);
				HAL_GPIO_WritePin(COIL_C_GPIO_Port, COIL_C_Pin, 0);
				break;
			case 3:
				HAL_GPIO_WritePin(COIL_B_GPIO_Port, COIL_B_Pin, 0);
				HAL_GPIO_WritePin(COIL_D_GPIO_Port, COIL_D_Pin, 1);
				HAL_GPIO_WritePin(COIL_A_GPIO_Port, COIL_A_Pin, 1);
				HAL_GPIO_WritePin(COIL_C_GPIO_Port, COIL_C_Pin, 0);
				break;
			case 4:
				HAL_GPIO_WritePin(COIL_B_GPIO_Port, COIL_B_Pin, 0);
				HAL_GPIO_WritePin(COIL_D_GPIO_Port, COIL_D_Pin, 0);
				HAL_GPIO_WritePin(COIL_A_GPIO_Port, COIL_A_Pin, 1);
				HAL_GPIO_WritePin(COIL_C_GPIO_Port, COIL_C_Pin, 0);
				break;
			case 5:
				HAL_GPIO_WritePin(COIL_B_GPIO_Port, COIL_B_Pin, 1);
				HAL_GPIO_WritePin(COIL_D_GPIO_Port, COIL_D_Pin, 0);
				HAL_GPIO_WritePin(COIL_A_GPIO_Port, COIL_A_Pin, 1);
				HAL_GPIO_WritePin(COIL_C_GPIO_Port, COIL_C_Pin, 0);
				break;
			case 6:
				HAL_GPIO_WritePin(COIL_B_GPIO_Port, COIL_B_Pin, 1);
				HAL_GPIO_WritePin(COIL_D_GPIO_Port, COIL_D_Pin, 0);
				HAL_GPIO_WritePin(COIL_A_GPIO_Port, COIL_A_Pin, 0);
				HAL_GPIO_WritePin(COIL_C_GPIO_Port, COIL_C_Pin, 0);
				break;
			case 7:
				HAL_GPIO_WritePin(COIL_B_GPIO_Port, COIL_B_Pin, 1);
				HAL_GPIO_WritePin(COIL_D_GPIO_Port, COIL_D_Pin, 0);
				HAL_GPIO_WritePin(COIL_A_GPIO_Port, COIL_A_Pin, 0);
				HAL_GPIO_WritePin(COIL_C_GPIO_Port, COIL_C_Pin, 1);
				break;
		}
        step = (step + 1) % 8; // Increment and wrap around the step counter
        HAL_Delay(1); // Adjust delay as needed for your motor
    }
}


void CheckDeviceState(){
	if (deviceState.mainMode == TIMER_MODE) {
		// indicate timer mode
		HAL_GPIO_WritePin(GPIOA, LD2_Pin, SET);
		HAL_GPIO_WritePin(GPIOB, LED_D1_Pin, SET);
		if (deviceState.timerMode == TIMER1) {
			LCD_SendString("Timer Mode:Tim1");
		} else if (deviceState.timerMode == TIMER2) {
			LCD_SendString("Timer Mode:Tim2");
		} else if (deviceState.timerMode == TIMER3) {
			LCD_SendString("Timer Mode:Tim3");
		} else {
			LCD_SendString("Timer Mode:Tim4");
		}
	} else {
		HAL_GPIO_WritePin(GPIOB, LED_D1_Pin, RESET);
		if (deviceState.clockMode == CLOCK) {
			HAL_GPIO_WritePin(GPIOA, LD2_Pin, RESET);
			int motor = 0;
			if (motor) {
				Motor(4096);
			}
			if (deviceState.modeState == DISPLAY) {
				// 	NOTE THIS CLOCK IS HARDCODED, YOU CANT GET REAL TIME UNLESS YOU USE
				// EXTERNAL SOURCE
				inMode = 1;
				DisplayClock();
				inMode = 0;
				LCD_Reset();
				reload = 1;
			} else {
				inMode = 1;
				ConfigClock();
				inMode = 0;
				LCD_Reset();
				reload = 1;
				deviceState.modeState = DISPLAY;
			}
		} else if (deviceState.clockMode == ALARM) {
			LCD_SendString("Clock Mode:Alarm");
		} else if (deviceState.clockMode == COUNTDOWN) {
			LCD_SendString("Clock Mode:Count");
		} else {
			inMode = 1;
			EnterStopwatch();
			inMode = 0;
			LCD_Reset();
			reload = 1;
		}
}


//// 	for testing
//	if (deviceState.modeState == DISPLAY) {
//		LCD_SetCursor(1, 0);
//		LCD_SendString("DISPLAY");
//	} else {
//		LCD_SetCursor(1, 0);
//		LCD_SendString("CONFIG");
//	}
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
