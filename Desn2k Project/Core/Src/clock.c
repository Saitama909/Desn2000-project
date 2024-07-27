/*
 * clock.c
 *
 *  Created on: Jul 27, 2024
 *      Author: zekro
 */
#include "lcd_keypad.h"
#include "stopwatch.h"
#include "main.h"
#include "stdbool.h"
#include "string.h"

const char* week_days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

static char last_day[3] = "  ";
static char last_month[3] = "  ";
static char last_year[5] = "    ";
static char last_weekday[4] = "   ";
static char last_hours[3] = "  ";
static char last_minutes[3] = "  ";
static char last_seconds[3] = "  ";
static char last_am_pm[3] = "  ";
int initial = 0;

uint8_t dec_to_bcd(uint8_t dec);
uint8_t bcd_to_dec(uint8_t bcd);
void Display_Date_Time();


void DisplayClock() {
	LCD_Reset();
	initial  = 1;
	Display_Date_Time();
	initial = 0;
	while(1) {
		if (hasStateChanged(deviceState)) {
			return;
		}
		Display_Date_Time();
	}
}

// Display date and time on the LCD
void Display_Date_Time()
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

    uint32_t day = bcd_to_dec(sDate.Date);
    uint32_t month = bcd_to_dec(sDate.Month);
    uint32_t year = 2000 + bcd_to_dec(sDate.Year);
    uint32_t hours = bcd_to_dec(sTime.Hours);
    uint32_t minutes = bcd_to_dec(sTime.Minutes);
    uint32_t seconds = bcd_to_dec(sTime.Seconds);
    uint32_t weekday = sDate.WeekDay;  // RTC weekday (1: Monday, ..., 7: Sunday)

    // Convert 24-hour format to 12-hour format with AM/PM
    char am_pm[3] = "AM";
    if (hours >= 12) {
        if (hours > 12) hours -= 12;
        am_pm[0] = 'P';
    }
    if (hours == 0) hours = 12;

    // Convert components to strings
    char day_str[4], month_str[4], year_str[5];
    char hour_str[4], minute_str[4], second_str[3];
    char weekday_str[4], am_pm_str[3];


    // Ignore Warnings
    snprintf(day_str, sizeof(day_str), "%02lu/", day);
    snprintf(month_str, sizeof(month_str), "%02lu/", month);
    snprintf(year_str, sizeof(year_str), "%04lu", year);
    snprintf(hour_str, sizeof(hour_str), "%02lu:", hours);
    snprintf(minute_str, sizeof(minute_str), "%02lu:", minutes);
    snprintf(second_str, sizeof(second_str), "%02lu", seconds);
    snprintf(weekday_str, sizeof(weekday_str), "%s", week_days[weekday]);
    snprintf(am_pm_str, sizeof(am_pm_str), "%s", am_pm);

    // Update each component if it has changed
    if (strcmp(day_str, last_day) != 0 || initial == 1) {
        LCD_SetCursor(0, 0); // Set cursor to day position
        LCD_SendString(day_str);
        strcpy(last_day, day_str);
    }

    if (strcmp(month_str, last_month) != 0 || initial == 1) {
        LCD_SetCursor(0, 3); // Set cursor to month position
        LCD_SendString(month_str);
        strcpy(last_month, month_str);
    }

    if (strcmp(year_str, last_year) != 0 || initial == 1) {
        LCD_SetCursor(0, 6); // Set cursor to year position
        LCD_SendString(year_str);
        strcpy(last_year, year_str);
    }

    if (strcmp(weekday_str, last_weekday) != 0 || initial == 1) {
        LCD_SetCursor(0, 11); // Set cursor to weekday position
        LCD_SendString(weekday_str);
        strcpy(last_weekday, weekday_str);
    }

    if (strcmp(hour_str, last_hours) != 0 || initial == 1) {
        LCD_SetCursor(1, 0); // Set cursor to hours position
        LCD_SendString(hour_str);
        strcpy(last_hours, hour_str);
    }

    if (strcmp(minute_str, last_minutes) != 0 || initial == 1) {
        LCD_SetCursor(1, 3); // Set cursor to minutes position
        LCD_SendString(minute_str);
        strcpy(last_minutes, minute_str);
    }

    if (strcmp(second_str, last_seconds) != 0 || initial == 1) {
        LCD_SetCursor(1, 6); // Set cursor to seconds position
        LCD_SendString(second_str);
        strcpy(last_seconds, second_str);
    }

    if (strcmp(am_pm_str, last_am_pm) != 0 || initial == 1) {
        LCD_SetCursor(1, 9); // Set cursor to AM/PM position
        LCD_SendString(am_pm_str);
        strcpy(last_am_pm, am_pm_str);
    }
}
uint8_t dec_to_bcd(uint8_t dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

uint8_t bcd_to_dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

