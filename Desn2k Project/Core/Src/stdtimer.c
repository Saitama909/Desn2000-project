/*
 * stdtimer.c
 *
 *  Created on: Jul 30, 2024
 *      Author: zekro
 */
#include "main.h"
#include "lcd_keypad.h"
#include "timer_config.h"
#include "clock.h"

volatile uint32_t duration = 0;
volatile uint32_t time_left = 0;
int alert_num = 1;
uint32_t prevseconds_LCD = -1;
volatile int timerRunning = 0;
int exitConfig;
int count = 0;
int timerDone = 0;
void ConfigTimer();
int EnterTimerDuration();
int CheckTimerDuration(int input_secs);
void DisplayTime(int input_secs);
void ChooseTimerAlert();
int ConvertToSeconds(int hhmmss);
void resetTimer(void);
void toggleTimer(void);
void DisplayTimer();
void stdTimerAlert();
void updateTimerLCD(uint32_t count);

// main timer display function
void DisplayTimer() {
	count ++;
	LCD_SendString("Standard Timer:");
	LCD_SetCursor(1, 0);
	prevseconds_LCD = -1;
	updateTimerLCD(time_left);
	while(1) {
		char input = scan_keypad();
		if (input == '#' && time_left != 0 && duration != 0) {
			// toggle timer
			toggleTimer();
			updateTimerLCD(time_left);
		} else if (input == '*') {
			// reset timer
			resetTimer();
			updateTimerLCD(time_left);
		} else if (timerRunning || timerDone == 1) {
			// update LCD when timer is done
			if (timerDone == 1) {
				timerDone = 0;
			}
			updateTimerLCD(time_left);
		}
		if (hasStateChanged(deviceState)) {
			return;
		}
	}
}

void toggleTimer(void) {
	timerRunning = !timerRunning;
	if (timerRunning) {
	    HAL_TIM_Base_Start_IT(&htim15);
	} else {
		HAL_TIM_Base_Stop_IT(&htim15);
	}
}

void resetTimer(void) {
    HAL_TIM_Base_Stop_IT(&htim15);
    time_left = duration;
    timerRunning = 0;
}


void updateTimerLCD(uint32_t count) {
    char buffer[17];
    uint32_t total_seconds = count;
    uint32_t hours = total_seconds / 3600;
    uint32_t minutes = (total_seconds % 3600) / 60;
    uint32_t seconds_LCD = total_seconds % 60;
    if (prevseconds_LCD == seconds_LCD) {
    		return;
	}
    prevseconds_LCD = seconds_LCD;
    snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu", hours, minutes, seconds_LCD);
    LCD_SetCursor(1, 0);
    LCD_SendString(buffer);
}

// main timer configuration function
void ConfigTimer() {
	exitConfig = 0;
	resetTimer();
	LCD_Clear();
	LCD_SetCursor(0, 0);
	LCD_SendString("Timer duration:");
	while(!EnterTimerDuration() && exitConfig == 0) {
	}

//	LCD_Clear();
//	LCD_SetCursor(0, 0);
//	LCD_SendString("Timer alert:");
//	ChooseTimerAlert();

	if (exitConfig == 1) {
		exitConfig = 0;
	}
	LCD_Clear();
	LCD_SetCursor(0, 0);
}

int EnterTimerDuration() {
	int input_secs = 0;
	LCD_SetCursor(1, 0);
	DisplayTime(input_secs);

	while (1) {
		char key = scan_keypad();
		if (key) {
			int num = key - '0';
			if (num >= 0 && num <= 9) {
				input_secs = input_secs * 10 + num;
				if (input_secs > 10000) {
					input_secs = 10000;
				}
				DisplayTime(input_secs);
			} else if (key == '#') {
				if(CheckTimerDuration(input_secs)) {
					return 1;
				}
				return 0;
			}
		}
		if (hasStateChanged(deviceState)) {
			exitConfig = 1;
			return 0;
		}
	}
}

// check if timer duration is valid
int CheckTimerDuration(int input_secs) {

	int total_secs = ConvertToSeconds(input_secs);

	if (total_secs < 30) {
		LCD_Clear();
		LCD_SetCursor(0, 0);
		LCD_SendString("Minimum time is");
		LCD_SetCursor(1, 0);
		LCD_SendString("30 seconds");
		HAL_Delay(1000);
		LCD_SendString("          ");
		return 0;

	} else if (total_secs > 3600) {
		LCD_Clear();
		LCD_SetCursor(0, 0);
		LCD_SendString("Maximum time is");
		LCD_SetCursor(1, 0);
		LCD_SendString("1 hour");
		HAL_Delay(1000);
		LCD_SendString("          ");
		return 0;

	} else {
		duration = total_secs;
		time_left = duration;
		return 1;
	}
}

int ConvertToSeconds(int hhmmss) {
    int hours = hhmmss / 10000;          // Extract hours
    int minutes = (hhmmss / 100) % 100;  // Extract minutes
    int seconds = hhmmss % 100;          // Extract seconds

    // Calculate the total seconds
    int total_seconds = (hours * 3600) + (minutes * 60) + seconds;
    return total_seconds;
}


void DisplayTime(int input_secs) {
    int hours = input_secs / 10000;
    int mins = (input_secs % 10000) / 100;
    int secs = input_secs % 100;

    char buffer[22] = "";
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, mins, secs);

    LCD_SetCursor(1, 0);
    LCD_SendString(buffer);
}

//void ChooseTimerAlert() {
//    init_alerts();
//
//    LCD_SetCursor(1, 0);
//    LCD_SendString("Pick song 1-6: ");
//
//    while (1) {
//		char key = scan_keypad();
//		if (key) {
//			int num = key - '0';
//			if (num >= 1 && num <= 6) {
//				LCD_SetCursor(1, 15);
//				LCD_Data(key);
//				play_alert(&songs[num - 1]);
//			} else if (key == '#') {
//				alert_num = num - 1;
//				break;
//			}
//		}
//	}
//}

void stdTimerAlert() {
	if (deviceState.clockMode == COUNTDOWN && deviceState.modeState == DISPLAY) {
		updateTimerLCD(time_left);
	}
	HAL_TIM_Base_Stop_IT(&htim15);
	Note note = {261, 500};
	TIM1->ARR = (72000000 / (note.freq * 1000)) - 1;
	TIM1->CCR3 = TIM1->ARR / 2;

	// NOTE: WE DO NOT WANT TO USE DELAYS - USE ANOTHER TIMER INSTEAD FOR THE DURATION OF THE NOTE
	__HAL_TIM_SET_AUTORELOAD(&htim16, note.duration * 10 - 1);
	__HAL_TIM_CLEAR_FLAG(&htim16, TIM_FLAG_UPDATE);

	HAL_TIM_Base_Start_IT(&htim16);
	note_playing = 1;
	while (note_playing) {

	}
	TIM1->CCR3 = 0;
//	play_alert(&songs[3]);
	for (int i = 0; i < 2; i++) {
		shiftByte(0b1111111111111111);
		latchData();
		HAL_Delay(500);
		shiftByte(0);
		latchData();
	}
	timerDone = 1;
}

