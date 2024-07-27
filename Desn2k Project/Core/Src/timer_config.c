/*
 * timer_config.c
 *
 *  Created on: Jul 26, 2024
 *      Author: Anna Yang
 */

#include "main.h"
#include "lcd_keypad.h"
#include "timer_config.h"

User user = {
	.state = CONFIGURE_TIMER_COUNT,
	.num_timers = 0
};

void welcome() {
	LCD_SendString("Hello!");
	LCD_SetCursor(1, 0);
	LCD_SendString("I am a lab timer");
	HAL_Delay(2000);

	LCD_Clear();
	LCD_SetCursor(0, 0);
	LCD_SendString("How many timers?");
}

// ONLY CALLED WHEN STATE == CONFIGURE_TIMER_COUNT
void choose_timer_count() {
	char num_timers = '\0';

	while (user.state == CONFIGURE_TIMER_COUNT) {
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
					num_timers -= '0';
					user.state = CONFIGURE_TIMER_DURATION;
					user.num_timers = num_timers;
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
}

// ONLY CALLED WHEN STATE == CONFIGURE_TIMER_DURATION
void config_timer_duration() {
	LCD_Clear();
	LCD_SetCursor(0, 0);
	for (int i = 0; i < user.num_timers; i++) {
		char buffer[17] = "";
		snprintf(buffer, sizeof(buffer), "Timer %d duration", i + 1);
		LCD_SendString(buffer);
		LCD_SetCursor(1, 0);
		enter_timer_duration(i);
	}
}

void enter_timer_duration(int timer_index) {
	char key = scan_keypad();
	int input_secs = 0;
	display_time(input_secs);

	while (1) {
		if (key) {
			int num = key = '0';
			if (num >= 0 && num <= 9) {
				input_secs = (input_secs * 10 + num) % 100000;
				display_time(input_secs);
			} else if (key == '#') {
				check_timer_duration(input_secs, timer_index);
				break;
			}
		}
	}
}

void check_timer_duration(int input_secs, int timer_index) {
	int total_secs = input_secs;

	if (total_secs < 30) {
		LCD_Clear();
		LCD_SetCursor(0, 0);
		LCD_SendString("Minimum time is");
		LCD_SetCursor(1, 0);
		LCD_SendString("30 seconds");
		HAL_Delay(1000);

		char buffer[17] = "";
		snprintf(buffer, sizeof(buffer), "Timer %d duration", timer_index + 1);
		LCD_SendString(buffer);
		LCD_SetCursor(1, 0);
		enter_timer_duration(timer_index);

	} else if (total_secs > 3600) {
		LCD_Clear();
		LCD_SetCursor(0, 0);
		LCD_SendString("Maximum time is");
		LCD_SetCursor(1, 0);
		LCD_SendString("1 hour");
		HAL_Delay(1000);

		char buffer[17] = "";
		snprintf(buffer, sizeof(buffer), "Timer %d duration", timer_index + 1);
		LCD_SendString(buffer);
		LCD_SetCursor(1, 0);
		enter_timer_duration(timer_index);

	} else {
		user.timers[timer_index].hours = total_secs / 3600;
		user.timers[timer_index].mins = (total_secs % 3600) / 60;
		user.timers[timer_index].secs = total_secs % 60;
	}
}

void display_time(int input_secs) {
	int hours = input_secs / 3600;
	int mins = (input_secs % 3600) / 60;
	int secs = input_secs % 60;

	char buffer[9];
	snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, mins, secs);

	LCD_SendString(buffer);
}
