/*
 * alarm.c
 *
 *  Created on: Jul 28, 2024
 *      Author: kaira
 */
#include "main.h"
#include "alarm.h"
#include "lcd_keypad.h"

/* Private variables ---------------------------------------------------------*/

static uint8_t alarmEnabled = 0; // 0 disabled, 1 enabled

/* Set Alarm function */
void SetAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds) {

    RTC_AlarmTypeDef sAlarm = {0};

    HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, FORMAT_BIN);

		sAlarm.AlarmTime.Hours = hours;
		sAlarm.AlarmTime.Minutes = minutes;
		sAlarm.AlarmTime.Seconds = seconds;

    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
}

/* Enable or disable alarm */
void EnableAlarm(uint8_t enable) {
    alarmEnabled = enable;
}

/* Snooze alarm for 3 minutes */
void SnoozeAlarm(void) {
		StopAlarm();
//    RTC_TimeTypeDef currentTime = {0};
//    HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BCD);
//
//    uint8_t newMinutes = currentTime.Minutes + 1;
//    uint8_t newHours = currentTime.Hours;
//
//    if (newMinutes >= 60) {
//        newMinutes -= 60;
//        newHours += 1;
//        if (newHours >= 24) {
//            newHours -= 24;
//        }
//    }
    RTC_AlarmTypeDef sAlarm = {0};

    HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, FORMAT_BIN);

    uint8_t hours = sAlarm.AlarmTime.Hours;
    uint8_t minutes =	sAlarm.AlarmTime.Minutes;
    uint8_t seconds = sAlarm.AlarmTime.Seconds;

    SetAlarm(hours, minutes + 3, seconds);
    EnableAlarm(1);
}

/* Stops the alarm */
void StopAlarm(void) {
		EnableAlarm(0);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		TIM1->CCR3 = 0;
}

void AlarmFunctionality() {
    uint8_t key = scan_keypad();
    if (key == 'A') {
    		LCD_SetCursor(1, 0);
    		LCD_SendString("Enabled ");
        EnableAlarm(1);
    } else if (key == 'B') {
  			LCD_SetCursor(1, 0);
  			LCD_SendString("Disabled");
        EnableAlarm(0);
    }
}

void DisplayAlarm() {
		LCD_SendString("Alarm");
		RTC_AlarmTypeDef sAlarm = {0};
		HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, FORMAT_BIN);
		char buffer[17];
		snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", sAlarm.AlarmTime.Hours, sAlarm.AlarmTime.Minutes, sAlarm.AlarmTime.Seconds);
		LCD_SetCursor(0, 7);
		LCD_SendString(buffer);
		while(1) {
				AlarmFunctionality();
				if (hasStateChanged(deviceState)) {
						return;
				}
		}
}

/* Get user input for the alarm */
void GetUserAlarmInput(void) {
    uint8_t hours = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;
    uint8_t key;


    // Set hours
    LCD_Clear();
    LCD_SetCursor(0,0);
    LCD_SendString("Alarm Config");
    LCD_SetCursor(1,0);
    LCD_SendString("Set Hours: ");
    while (1) {
        key = scan_keypad();
        if (key != 0xFF) {
            if (key >= '0' && key <= '9') {
                hours = (hours * 10) + (key - '0');
                LCD_Data(key);
            } else if (key == '#') {
                break;
            }
        }
        if (hasStateChanged(deviceState)) {
						return;
				}
    }

    // Set minutes
    LCD_ClearLine(1);
    LCD_SetCursor(1,0);
    LCD_SendString("Set Minutes: ");
    while (1) {
        key = scan_keypad();
        if (key != 0xFF) {
            if (key >= '0' && key <= '9') {
                minutes = (minutes * 10) + (key - '0');
                LCD_Data(key);
            } else if (key == '#') {
                break;
            }
        }
        if (hasStateChanged(deviceState)) {
            return;
        }
    }

    // Set seconds
    LCD_ClearLine(1);
    LCD_SetCursor(1,0);
    LCD_SendString("Set Seconds: ");
    while (1) {
        key = scan_keypad();
        if (key != 0xFF) {
            if (key >= '0' && key <= '9') {
                seconds = (seconds * 10) + (key - '0');
                LCD_Data(key);
            } else if (key == '#') {
                break;
            }
        }
        if (hasStateChanged(deviceState)) {
        		return;
        }
    }

    SetAlarm(hours, minutes, seconds);
    EnableAlarm(1);

}


void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {

			if (alarmEnabled){
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
					TIM1->ARR = 299;
					TIM1->CCR3 = TIM1->ARR / 2;
					// Adds a delay
					for(uint32_t i = 0; i < 0x0FF; i++){
							for(uint32_t j = 0; j < 0x0FF; j++){

							}
					}
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
					TIM1->CCR3 = 0;

					EnableAlarm(0);
			}

}
