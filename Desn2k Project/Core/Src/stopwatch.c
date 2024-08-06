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

volatile uint32_t seconds = 0;
int stopwatchRunning = 0;
uint32_t prevseconds;
uint32_t lapTimes[2] = {0, 0}; // Array to store up to two lap times
int lapCount = 0; // Count of laps taken
int stopwatchStart;

void toggleStopwatch(void);
void updateLapDisplay();
void lapStopwatch(void);

// main stopwatch function
void EnterStopwatch() {
	LCD_Reset();
	LCD_SendString("S-watch");
	stopwatchStart = 1;
	updateLCD(seconds);
	prevseconds = -1;
	while(1) {
		char input = scan_keypad();
		if (input == '#') {
			toggleStopwatch();
		} if (input == '*') {
			resetStopwatch();
			LCD_SendString("S-watch");
			stopwatchStart = 1;
			updateLCD(seconds);
		} if (input == '0') {
			lapStopwatch();
		}
		if (stopwatchRunning) {
			updateLCD(seconds);
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
    uint32_t total_seconds = count;
    uint32_t hours = total_seconds / 3600;
    uint32_t minutes = (total_seconds % 3600) / 60;
    uint32_t seconds_LCD = total_seconds % 60;
    if (prevseconds == seconds_LCD && !stopwatchStart) {
    		return;
	}
    prevseconds = seconds_LCD;
    snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu", hours, minutes, seconds_LCD);
    LCD_SetCursor(0, 8);
    LCD_SendString(buffer);

    // only runs once at the start of entering stopwatch
    if (stopwatchStart) {
    	stopwatchStart = 0;
    }
}

// toggles the stopwatch
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
    seconds = 0;
    stopwatchRunning = false;
    lapTimes[1] = 0;
	lapTimes[2] = 0;
	lapCount = 0;
	LCD_Clear();
}

// lap functionality
void lapStopwatch(void) {
	// only allows 2 laps
    if (lapCount < 2) {
        lapTimes[lapCount] = seconds;
        lapCount++;
    }
    updateLapDisplay();
}

// update the lap screen
void updateLapDisplay() {
    if (lapCount > 0) {
        char lapBuffer[9];
        snprintf(lapBuffer, sizeof(lapBuffer), "%02lu:%02lu:%02lu", lapTimes[0] / 3600, (lapTimes[0] % 3600) / 60, lapTimes[0] % 60);
        LCD_SetCursor(1, 0);
        LCD_SendString(lapBuffer);
    }
    if (lapCount > 1) {
        char lapBuffer[9];
        snprintf(lapBuffer, sizeof(lapBuffer), "%02lu:%02lu:%02lu", lapTimes[1] / 3600, (lapTimes[1] % 3600) / 60, lapTimes[1] % 60);
        LCD_SetCursor(1, 8);
        LCD_SendString(lapBuffer);
    }
}
