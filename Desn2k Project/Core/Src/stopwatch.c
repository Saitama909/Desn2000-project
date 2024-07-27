/*
 * stopwatch.c
 *
 *  Created on: Jul 27, 2024
 *      Author: William Chan
 */

#include "lcd_keypad.h"
#include "stopwatch.h"
#include "main.h"
#include "stdbool.h"

volatile uint32_t milliseconds = 0;
int stopwatchRunning = 0;

void EnterStopwatch() {
	int counter = 0;
	LCD_Reset();
	while(1) {
		char input = scan_keypad();
		if (input != '\0') {
			if (input == '#') {
				HAL_GPIO_WritePin(GPIOA, LD2_Pin, SET);
				toggleStopwatch();
				updateLCD(milliseconds);
			}
		}
		if (stopwatchRunning) {
			updateLCD(milliseconds);
			HAL_GPIO_WritePin(GPIOA, LD2_Pin, RESET);
		}
	}
}

void LCD_Reset() {
	LCD_Clear();
	LCD_SetCursor(0, 0);
}

void updateLCD(uint32_t count) {
	char buffer[17];
	uint32_t seconds = count / 1000;
	uint32_t minutes = seconds / 60;
	seconds %= 60;
	uint32_t milli = count % 1000;

	snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%03lu", minutes, seconds, milli);
	LCD_SetCursor(0, 0);
	LCD_SendString(buffer);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	milliseconds++;
}


void toggleStopwatch(void) {
	stopwatchRunning = !stopwatchRunning;
	if (stopwatchRunning) {
	    HAL_TIM_Base_Start_IT(&htim6);
	} else {
		HAL_TIM_Base_Stop_IT(&htim6);
	}
}

void resetStopwatch(void) {
    HAL_TIM_Base_Stop_IT(&htim6);
    milliseconds = 0;
    stopwatchRunning = false;
}

