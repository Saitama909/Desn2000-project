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
uint32_t prevseconds;

void toggleStopwatch(void);

void EnterStopwatch() {
	LCD_Reset();
	updateLCD(milliseconds);
	prevseconds = -1;
	while(1) {
		char input = scan_keypad();
		if (input == '#') {
			toggleStopwatch();
		} if (input == '*') {
			resetStopwatch();
		}
		updateLCD(milliseconds);
		if (stopwatchRunning) {
			updateLCD(milliseconds);
			HAL_GPIO_WritePin(GPIOA, LD2_Pin, RESET);
		}
		if (hasStateChanged(deviceState)) {
			return;
		}
	}
}

void LCD_Reset() {
	LCD_Clear();
	LCD_SetCursor(0, 0);
}

void updateLCD(uint32_t count) {
    char buffer[17];
    uint32_t total_seconds = count / 1000;
    uint32_t hours = total_seconds / 3600;
    uint32_t minutes = (total_seconds % 3600) / 60;
    uint32_t seconds = total_seconds % 60;
    if (prevseconds == seconds) {
    		return;
	}
    prevseconds = seconds;

    snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu", hours, minutes, seconds);
    LCD_SetCursor(0, 0);
    LCD_SendString(buffer);
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

