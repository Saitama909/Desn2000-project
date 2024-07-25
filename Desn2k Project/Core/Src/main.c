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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define D_PORT GPIOC
#define LCD_D4_PIN GPIO_PIN_8
#define LCD_D5_PIN GPIO_PIN_9
#define LCD_D6_PIN GPIO_PIN_10
#define LCD_D7_PIN GPIO_PIN_11
#define E_PORT GPIOD
#define RW_PORT GPIOC
#define RS_PORT GPIOA
#define LCD_E_PIN GPIO_PIN_2
#define LCD_RW_PIN GPIO_PIN_6
#define LCD_RS_PIN GPIO_PIN_15

#define C_PORT GPIOA
#define R_PORT GPIOB

#define C1_PIN GPIO_PIN_8
#define C2_PIN GPIO_PIN_9
#define C3_PIN GPIO_PIN_10
#define C4_PIN GPIO_PIN_11

#define R1_PIN GPIO_PIN_11
#define R2_PIN GPIO_PIN_12
#define R3_PIN GPIO_PIN_13
#define R4_PIN GPIO_PIN_14
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t key_map[4][4] = {{'1','2','3','A'},
                         {'4','5','6','B'},
                         {'7','8','9','C'},
                         {'*','0','#','D'}};
uint16_t colpins[4] = {C1_PIN, C2_PIN, C3_PIN, C4_PIN};
uint16_t rowpins[4] = {R1_PIN, R2_PIN, R3_PIN, R4_PIN};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void init_magic();
void LCD_Pulse();
void LCD_PutNibble(uint8_t nibble);
void LCD_Data(uint8_t c);
void LCD_Command(uint8_t c);
void LCD_SendString(char* str);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Clear(void);
uint8_t scan_keypad();
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
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_GPIO_WritePin(RW_PORT, LCD_RW_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(E_PORT, LCD_E_PIN, GPIO_PIN_RESET);

  init_magic();

  LCD_SendString("Hello!");
  LCD_SetCursor(1, 0);
  LCD_SendString("I am a lab timer");
  HAL_Delay(2000);

  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_SendString("How many timers?");

  char num_timers = '\0';

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  char key = scan_keypad();

	  if (key) {
		  if (key != '#') {
			  LCD_SetCursor(1, 0);
			  LCD_Data(key);
			  num_timers = key;
		  } else {
			  // Enter key pressed
			  LCD_Clear();
			  LCD_SetCursor(0, 0);

			  if (num_timers == '1' || num_timers == '2' || num_timers == '3' || num_timers == '4') {
				  // VALID INPUT
				  LCD_SendString("yippee");
			  } else {
				  // INVALID INPUT
				  if (num_timers == '\0') {
					  LCD_SendString("You must enter");
					  LCD_SetCursor(1, 0);
					  LCD_SendString("a number");
				  } else {
					  LCD_SendString("Max 4 timers");
				  }

				  // Ask user again for number of timers
				  HAL_Delay(1000);
				  LCD_Clear();
				  LCD_SetCursor(0, 0);
				  LCD_SendString("How many timers?");

				  // Reset number of timers
				  num_timers = '\0';
			  }
		  }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

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

  GPIO_InitStruct.Pin = C1_PIN | C2_PIN | C3_PIN | C4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(C_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = R1_PIN | R2_PIN | R3_PIN | R4_PIN;
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
void LCD_Pulse() {
    HAL_GPIO_WritePin(E_PORT, LCD_E_PIN, GPIO_PIN_SET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(E_PORT, LCD_E_PIN, GPIO_PIN_RESET);
    HAL_Delay(5);
}

void LCD_PutNibble(uint8_t nibble) {
    HAL_GPIO_WritePin(D_PORT, LCD_D4_PIN, (nibble & 0x01));
    HAL_GPIO_WritePin(D_PORT, LCD_D5_PIN, ((nibble >> 1) & 0x01));
    HAL_GPIO_WritePin(D_PORT, LCD_D6_PIN, ((nibble >> 2) & 0x01));
    HAL_GPIO_WritePin(D_PORT, LCD_D7_PIN, ((nibble >> 3) & 0x01));
}

void LCD_Data(uint8_t c) {
    HAL_GPIO_WritePin(RS_PORT, LCD_RS_PIN, GPIO_PIN_SET);
    LCD_PutNibble(c >> 4);
    LCD_Pulse();
    LCD_PutNibble(c & 0x0F);
    LCD_Pulse();
    HAL_GPIO_WritePin(RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
}

void LCD_Command(uint8_t c) {
    HAL_GPIO_WritePin(RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    LCD_PutNibble(c >> 4);
    LCD_Pulse();
    LCD_PutNibble(c & 0x0F);
    LCD_Pulse();
}

void init_magic() {
    HAL_Delay(50);
    LCD_PutNibble(0x03);
    LCD_Pulse();
    HAL_Delay(5);
    LCD_PutNibble(0x03);
    LCD_Pulse();
    HAL_Delay(1);
    LCD_PutNibble(0x03);
    LCD_Pulse();
    LCD_PutNibble(0x02);
    LCD_Pulse();
    LCD_Command(0x28);
    LCD_Command(0x08);
    LCD_Command(0x01);
    LCD_Command(0x06);
    LCD_Command(0x0C);
}

void LCD_SendString(char* str) {
    while (*str) {
        LCD_Data(*str++);
    }
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x00 : 0x40;
    address += col;
    LCD_Command(0x80 | address);
}

void LCD_Clear(void) {
    LCD_Command(0x01);
    HAL_Delay(5);
}

uint8_t scan_keypad() {
    uint8_t ret = '\0';
    for (int c = 0; c < 4; c++) {
        HAL_GPIO_WritePin(C_PORT, colpins[c], GPIO_PIN_SET);
        for (int r = 0; r < 4; r++) {
            if (HAL_GPIO_ReadPin(R_PORT, rowpins[r])) {
                while (HAL_GPIO_ReadPin(R_PORT, rowpins[r])) {
                }
                HAL_Delay(50);
                ret = key_map[r][c];
            }
        }
        HAL_GPIO_WritePin(C_PORT, colpins[c], GPIO_PIN_RESET);
    }
    return ret;
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
