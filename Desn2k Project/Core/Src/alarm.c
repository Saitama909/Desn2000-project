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

/* Set Alarm Function */
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

/* Enable or Disable Alarm */
void EnableAlarm(uint8_t enable) {
    alarmEnabled = enable;
}

/* Snooze Alarm for 1 minutes */
void SnoozeAlarm(void) {
    RTC_TimeTypeDef currentTime = {0};
    HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BCD);

    uint8_t newMinutes = currentTime.Minutes + 1;
    uint8_t newHours = currentTime.Hours;

    if (newMinutes >= 60) {
        newMinutes -= 60;
        newHours += 1;
        if (newHours >= 24) {
            newHours -= 24;
        }
    }

    SetAlarm(newHours, newMinutes, currentTime.Seconds);
    EnableAlarm(1);
    LCD_Clear();
    LCD_SendString("Snoozed!");
}

/* Stops the alarm */
void StopAlarm(void) {
		EnableAlarm(0);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		TIM1->CCR3 = 0;
		LCD_Clear();
		LCD_SendString("Alarm Stopped");
}

void AlarmFunctionality() {
    uint8_t key = scan_keypad();
    if (key == 'A') {
    		LCD_SetCursor(1, 0);
    		LCD_SendString("Enabled");
        EnableAlarm(1);
    } else if (key == 'B') {
  			LCD_SetCursor(1, 0);
  			LCD_SendString("Disabled");
        EnableAlarm(0);
    } else if (key == 'C') {
        SnoozeAlarm();
    } else if (key == 'D') {
        StopAlarm();
    }
}

void DisplayAlarm() {
		LCD_SendString("Alarm");
		while(1) {
				AlarmFunctionality();
				if (hasStateChanged(deviceState)) {
						return;
				}
		}
}

/* Get User Alarm Input Function */
void GetUserAlarmInput(void) {
    uint8_t hours = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;
    uint8_t key;


    // Set Hours
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

    // Set Minutes
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

    // Set Seconds
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
					// adds a delay
					for(uint32_t i = 0; i < 0xFFF; i++){
							for(uint32_t j = 0; j < 0xFFF; j++){

							}
					}
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
					TIM1->CCR3 = 0;

					EnableAlarm(0);
			}

}

// use the exti when button is pressed
