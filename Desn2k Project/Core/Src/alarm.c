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
static uint8_t alarmEnabled = 0;

/* Set Alarm Function */
void Set_Alarm(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    RTC_AlarmTypeDef sAlarm = {0};

//    if (am_pm == 1 && hours != 12) {
//        hours += 12;
//    } else if (am_pm == 0 && hours == 12) {
//        hours = 0;
//    }

    		HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, FORMAT_BIN);
    		sAlarm.AlarmTime.Seconds += 1;
    			while(HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, FORMAT_BIN) != HAL_OK);
//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
HAL_RTC_AlarmAEventCallback(&hrtc);

//    sAlarm.AlarmTime.Hours = hours;
//    sAlarm.AlarmTime.Minutes = minutes;
//
//    sAlarm.AlarmTime.Seconds = seconds;
//    sAlarm.AlarmTime.SubSeconds = 0x0;
//    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
//    sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
//    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
//    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
//    sAlarm.AlarmDateWeekDay = 0x1;
//    sAlarm.Alarm = RTC_ALARM_A;
//
//    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
//    {
//        Error_Handler();
//    }
}

/* Enable or Disable Alarm */
void Enable_Alarm(uint8_t enable) {
    alarmEnabled = enable;
}

/* Snooze Alarm for 5 minutes */
void Snooze_Alarm(void) {
    RTC_TimeTypeDef currentTime = {0};
    HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BCD);

    uint8_t newMinutes = currentTime.Minutes + 5;
    uint8_t newHours = currentTime.Hours;

    if (newMinutes >= 60) {
        newMinutes -= 60;
        newHours += 1;
        if (newHours >= 24) {
            newHours -= 24;
        }
    }

    Set_Alarm(newHours, newMinutes, currentTime.Seconds);
}

/* Stop Alarm */
void Stop_Alarm(void) {
	alarmEnabled = 0;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);  // Turn off the buzzer
	LCD_Clear();
	LCD_SendString("Alarm Stopped");
}

/* RTC Alarm A Event Callback */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
//		RTC_AlarmTypeDef sAlarm;
//		HAL_RTC_GetAlarm(hrtc, &sAlarm, RTC_ALARM_A, FORMAT_BIN);
//		sAlarm.AlarmTime.Seconds += 1;
//			while(HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, FORMAT_BIN) != HAL_OK);
//		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
				   // if (alarmEnabled) {
    	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);  // Turn on the buzzer
//			LCD_Clear();
//			LCD_SendString("Alarm Triggered!");
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

			// Play a sound using the buzzer
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
			for (int i = 0; i < 3; i++) {
					TIM1->CCR1 = 1000; // Adjust duty cycle to generate sound
					HAL_Delay(500);
					TIM1->CCR1 = 0; // Turn off sound
					HAL_Delay(500);
			}
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);

			// Stop the buzzer after the sound
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    //}
}

/* Get User Alarm Input Function */
void Get_User_Alarm_Input(void) {
    uint8_t hours = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;
//    uint8_t am_pm = 0;
    uint8_t key;

    // Set Hours
    LCD_Clear();
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
    }

    // Set Minutes
    LCD_Clear();
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
    }

    // Set Seconds
    LCD_Clear();
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
    }

    // Set AM/PM
//    LCD_Clear();
//    LCD_SendString("Set AM/PM (0/1): ");
//    while (1) {
//        key = scan_keypad();
//        if (key != 0xFF) {
//            if (key == '0' || key == '1') {
//                am_pm = key - '0';
//                LCD_Data(key);
//                break;
//            }
//        }
//    }

    Set_Alarm(hours, minutes, seconds);
    Enable_Alarm(1);

    LCD_Clear();
    LCD_SendString("Alarm Set!");
}

