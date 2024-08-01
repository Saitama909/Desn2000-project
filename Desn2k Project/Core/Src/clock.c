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

#define DAY 0
#define MONTH 1
#define YEAR 2
#define HOUR 3
#define MINUTE 4
#define SECOND 5
#define WEEKDAY 6
#define THIRTYDEGREE 341

const char* week_days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
const int positions[7] = {0, 3, 6, 16, 19, 22, 11};

static char last_day[3] = "  ";
static char last_month[3] = "  ";
static char last_year[5] = "    ";
static char last_weekday[4] = "   ";
static char last_hours[3] = "  ";
static char last_minutes[3] = "  ";
static char last_seconds[3] = "  ";
static char last_am_pm[3] = "  ";
int initial;
int pos;
uint32_t buffer[7];
int first_time_running = 1;
int last_hour;
uint8_t dec_to_bcd(uint8_t dec);
uint8_t bcd_to_dec(uint8_t bcd);
void DisplayDateTime();
void EditTime();
void IncreaseSelection();
void GetCurrentTime();
void DisplayEditedTime();
void DecreaseSelection();
void SetTime();
bool IsValidDate(uint8_t day, uint8_t month, uint8_t year);
bool IsLeapYear(uint8_t year);
void Flash();
void shiftBit(uint8_t bit);
void shiftByte(uint16_t data);
void latchData(void);
void LightShiftLED();
uint16_t getBitPattern(uint8_t position);

void DisplayClock() {
	LCD_Reset();
	// as DisplayDateTime only displays if different so initially we want it to display
	initial = 1;
	DisplayDateTime();
	while(1) {
		if (hasStateChanged(deviceState)) {
			return;
		}
		DisplayDateTime();
		LightShiftLED();
	}
}

void LightShiftLED() {
	RTC_TimeTypeDef sTime = {0};
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	uint32_t hours = bcd_to_dec(sTime.Hours);
//	uint32_t minutes = bcd_to_dec(sTime.Minutes);
//	uint32_t seconds = bcd_to_dec(sTime.Seconds);
	uint16_t pattern = getBitPattern(hours);
	shiftByte(pattern);
	latchData();
}

uint16_t getBitPattern(uint8_t position) {
    if (position == 0) {
        return 0b0000000000000000;
    } else if (position <= 12) {
        return ((1 << position) - 1) << 4;  // Ensure the first 4 bits are 0
    } else {
        return ((1 << (24 - position)) - 1) << 4;  // Ensure the first 4 bits are 0
    }
}





void shiftBit(uint8_t bit) {
  if (bit) {
    HAL_GPIO_WritePin(GPIOB, SER_Pin, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(GPIOB, SER_Pin, GPIO_PIN_RESET);
  }

  // Pulse the clock
  HAL_GPIO_WritePin(GPIOC, SRCLK_Pin, GPIO_PIN_SET);
  HAL_Delay(1); // Short delay
  HAL_GPIO_WritePin(GPIOC, SRCLK_Pin, GPIO_PIN_RESET);
  HAL_Delay(1); // Short delay
}

void shiftByte(uint16_t data) {
  for (int i = 0; i <= 15; i++) {
    shiftBit((data >> i) & 0x01);
  }
}


void latchData(void) {
  HAL_GPIO_WritePin(GPIOB, RCLK_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(GPIOB, RCLK_Pin, GPIO_PIN_RESET);
  HAL_Delay(1);
}

void ConfigClock() {
	LCD_Reset();
	for (int i = 0; i < 7; i++) {
		buffer[i] = 0;
	}
	EditTime();
	DisplayDateTime();
	return;
}


void EditTime() {
	int flashCounter = 0;
	pos = 0;
	GetCurrentTime();
	initial = 1;
	DisplayDateTime();
	while (1) {
		char key = scan_keypad();
		if (key != '\0') {
			// move position of selection
			if (key == 'D') {
				if (pos == 6) {
					if (IsValidDate(buffer[DAY], buffer[MONTH], buffer[YEAR])) {
						SetTime();
						LCD_Reset();
						LCD_SendString("Time Changed!");
						HAL_Delay(3000);
						LCD_Reset();
						initial = 1;
					} else {
						LCD_Reset();
						LCD_SendString("Invalid Date!");
						HAL_Delay(3000);
						LCD_Reset();
						initial = 1;
					}
					return;
				}
				pos++;
			} else if (key == 'C') {
				if (pos > 0) {
					pos--;
				}
			}
			// increase/decrease number
			else if (key == 'A') {
				IncreaseSelection();
				DisplayEditedTime();
			} else if (key == 'B') {
				DecreaseSelection();
				DisplayEditedTime();
			}
		}
		if (flashCounter == 50000) {
			flashCounter = 0;
			Flash();
		}
		flashCounter++;
		if (hasStateChanged(deviceState)) {
			return;
		}
	}
}

void Flash() {
	char day_str[4], month_str[4], year_str[5];
	char hour_str[4], minute_str[4], second_str[3];
	char weekday_str[4];
	int row = 0, col = 0;
	int cur = positions[pos];
	if (cur > 15) {
		cur -= 16;
		row = 1;
	}
	col = cur;
	LCD_SetCursor(row, col);
	if (pos == DAY) {
		LCD_SendString("  ");
		snprintf(day_str, sizeof(day_str), "%02lu/", buffer[DAY]);
		LCD_SetCursor(row, col);
		LCD_SendString(day_str);
	} else if (pos == MONTH) {
		LCD_SendString("  ");
		snprintf(month_str, sizeof(month_str), "%02lu/", buffer[MONTH]);
		LCD_SetCursor(row, col);
		LCD_SendString(month_str);
	} else if (pos == YEAR) {
		LCD_SendString("    ");
		snprintf(year_str, sizeof(year_str), "%04lu", buffer[YEAR]);
		LCD_SetCursor(row, col);
		LCD_SendString(year_str);
	} else if (pos == HOUR) {
		LCD_SendString("  ");
		int pm_time = buffer[3];
		// Convert 24-hour format to 12-hour format with AM/PM
		if (buffer[3] >= 12) {
			if (buffer[3] > 12) pm_time -= 12;
		}
		if (buffer[3] == 0) pm_time = 12;
		snprintf(hour_str, sizeof(hour_str), "%02lu", pm_time);
		LCD_SetCursor(row, col);
		LCD_SendString(hour_str);
	} else if (pos == MINUTE) {
		LCD_SendString("  ");
		snprintf(minute_str, sizeof(minute_str), "%02lu", buffer[MINUTE]);
		LCD_SetCursor(row, col);
		LCD_SendString(minute_str);
	} else if (pos == SECOND) {
		LCD_SendString("  ");
		snprintf(second_str, sizeof(second_str), "%02lu", buffer[SECOND]);
		LCD_SetCursor(row, col);
		LCD_SendString(second_str);
	} else if (pos == WEEKDAY) {
		LCD_SendString("   ");
		snprintf(weekday_str, sizeof(weekday_str), "%s", week_days[buffer[6] - 1]);
		LCD_SetCursor(row, col);
		LCD_SendString(weekday_str);
	}
}

void SetTime() {
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	sDate.Date = dec_to_bcd(buffer[DAY]);
	sDate.Month = dec_to_bcd(buffer[MONTH]);
	sDate.Year = dec_to_bcd(buffer[YEAR] - 2000);
	sDate.WeekDay = buffer[WEEKDAY];

	sTime.Hours = dec_to_bcd(buffer[HOUR]);
	sTime.Minutes = dec_to_bcd(buffer[MINUTE]);
	sTime.Seconds = dec_to_bcd(buffer[SECOND]);
	sTime.TimeFormat = RTC_HOURFORMAT12_AM;

	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}
void GetCurrentTime() {
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

	buffer[0] = bcd_to_dec(sDate.Date);
	buffer[1] = bcd_to_dec(sDate.Month);
	buffer[2] = 2000 + bcd_to_dec(sDate.Year);
	buffer[3] = bcd_to_dec(sTime.Hours);
	buffer[4] = bcd_to_dec(sTime.Minutes);
	buffer[5] = bcd_to_dec(sTime.Seconds);
	buffer[6] = sDate.WeekDay;
}

void DisplayEditedTime() {
		int pm_time = buffer[3];
		// Convert 24-hour format to 12-hour format with AM/PM
		char am_pm[3] = "AM";
		if (buffer[3] >= 12) {
			if (buffer[3] > 12) pm_time -= 12;
			am_pm[0] = 'P';
		}
		if (buffer[3] == 0) pm_time = 12;
		// Convert components to strings
		    char day_str[4], month_str[4], year_str[5];
		    char hour_str[4], minute_str[4], second_str[3];
		    char weekday_str[4], am_pm_str[3];

		    // Ignore Warnings
		    snprintf(day_str, sizeof(day_str), "%02lu/", buffer[0]);
		    snprintf(month_str, sizeof(month_str), "%02lu/", buffer[1]);
		    snprintf(year_str, sizeof(year_str), "%04lu", buffer[2]);
		    snprintf(hour_str, sizeof(hour_str), "%02d:", pm_time);
		    snprintf(minute_str, sizeof(minute_str), "%02lu:", buffer[4]);
		    snprintf(second_str, sizeof(second_str), "%02lu", buffer[5]);
		    snprintf(weekday_str, sizeof(weekday_str), "%s", week_days[buffer[6] - 1]);
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

		    initial = 0;
}

void IncreaseSelection() {
	if (pos == DAY) {
		if (buffer[DAY] < 31) {
			buffer[DAY]++;
		} else {
			buffer[DAY] = 1;
		}
	} else if (pos == MONTH) {
		if (buffer[MONTH] < 12) {
			buffer[MONTH]++;
		} else {
			buffer[MONTH] = 1;
		}
	} else if (pos == YEAR) {
		if (buffer[YEAR] < 2099) {
			buffer[YEAR]++;
		} else {
			buffer[YEAR] = 2000;
		}
	} else if (pos == HOUR) {
		if (buffer[HOUR] < 23) {
			buffer[HOUR]++;
		} else {
			buffer[HOUR] = 0;
		}
	} else if (pos == MINUTE) {
		if (buffer[MINUTE] < 60) {
			buffer[MINUTE]++;
		} else {
			buffer[MINUTE] = 0;
		}
	} else if (pos == SECOND){
		if (buffer[SECOND] < 60) {
			buffer[SECOND]++;
		} else {
			buffer[SECOND] = 0;
		}
	} else {
		if (buffer[WEEKDAY] < 7) {
			buffer[WEEKDAY]++;
		} else {
			buffer[WEEKDAY] = 1;
		}
	}
}

void DecreaseSelection() {
	if (buffer[pos] > 0) {
		if (pos != WEEKDAY) {
			buffer[pos]--;
		} else if (buffer[pos] > 1){
			buffer[pos]--;
		}
	}
}
// Display date and time on the LCD
void DisplayDateTime()
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
    snprintf(weekday_str, sizeof(weekday_str), "%s", week_days[weekday - 1]);
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

    if (first_time_running == 1) {
    	last_hour = hours;
        Motor(THIRTYDEGREE * hours);
        first_time_running = 0;
    } else {
    	if (last_hour != hours) {
    		if (hours > 12) {
    			hours -= 12;
    		}
    		if (last_hour > hours) {
    			Motor(THIRTYDEGREE * (12 - last_hour + hours));
    		} else {
    			Motor(THIRTYDEGREE * (hours - last_hour));
    		}
    		last_hour = hours;
    	}
    }

    if (initial == 1) {
    	initial = 0;
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

const uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool IsLeapYear(uint8_t year) {
    // Check if the year is a leap year
    year += 2000;  // Adjust to the full year
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

bool IsValidDate(uint8_t day, uint8_t month, uint8_t year) {
    if (month < 1 || month > 12) {
        return false;
    }

    uint8_t max_days = days_in_month[month - 1];
    if (month == 2 && IsLeapYear(year)) {
        max_days = 29;
    }

    if (day < 1 || day > max_days) {
        return false;
    }

    return true;
}

