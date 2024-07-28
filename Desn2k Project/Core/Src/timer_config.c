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

KeyMap t9key_map[] = {
    {'2', "abc"},
    {'3', "def"},
    {'4', "ghi"},
    {'5', "jkl"},
    {'6', "mno"},
    {'7', "pqrs"},
    {'8', "tuv"},
    {'9', "wxyz"},
    {'0', " "},
    {'#', ""}
};

char current_key = '\0';
int current_char_index = 0;
uint32_t last_key_time = 0;

void welcome() {
	LCD_SendString("Hello!");
	LCD_SetCursor(1, 0);
	LCD_SendString("I am a lab timer");
	HAL_Delay(2000);

	LCD_Clear();
	LCD_SetCursor(0, 0);
	LCD_SendString("How many timers?");

	choose_timer_count();
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

	config_specific_timer();
}

// ONLY CALLED WHEN STATE == CONFIGURE_TIMER_DURATION
void config_specific_timer() {
	LCD_Clear();
	LCD_SetCursor(0, 0);

	// Cycle through specified number of timers
	for (int i = 0; i < user.num_timers; i++) {
		char buffer[22] = "";
		snprintf(buffer, sizeof(buffer), "Timer %d duration", i + 1);
		LCD_SendString(buffer);
		enter_timer_duration(i);

		user.state = CONFIGURE_TIMER_NAME;
		LCD_Clear();
		LCD_SetCursor(0, 0);
		snprintf(buffer, sizeof(buffer), "Timer %d name", i + 1);
		LCD_SendString(buffer);
		enter_timer_name(i);
	}
}

void enter_timer_duration(int timer_index) {
	int input_secs = 0;
	LCD_SetCursor(1, 0);
	display_time(input_secs);

	while (1) {
		char key = scan_keypad();
		if (key) {
			int num = key - '0';
			if (num >= 0 && num <= 9) {
				input_secs = input_secs * 10 + num;
				display_time(input_secs);
			} else if (key == '#') {
				check_timer_duration(input_secs, timer_index);
				break;
			}
		}
	}
}

void display_time(int input_secs) {
	int hours = input_secs / 10000;
	int mins = (input_secs % 10000) / 100;
	int secs = input_secs % 100;

	char buffer[22] = "";
	snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, mins, secs);

	LCD_SetCursor(1, 0);
	LCD_SendString(buffer);
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

		LCD_Clear();
		char buffer[22] = "";
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

		LCD_Clear();
		char buffer[22] = "";
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

void enter_timer_name(int timer_index) {
	LCD_SetCursor(1, 0);
	char input_text[17] = "";

	while (1) {
		char key = scan_keypad();
		if (key) {
			if (key == '#') {
				strncpy(user.timers[timer_index].name, input_text, 16);
				user.timers[timer_index].name[16] = '\0';
				break;
			} else {
				t9_typing(key, input_text);
			}
		}
		HAL_Delay(100);
	}

	LCD_Clear();
}

void t9_typing(int key, char *input_text) {
	uint32_t current_time = HAL_GetTick();

	// Backspace
	if (key == '*') {
		if (strlen(input_text) > 0) {
			input_text[strlen(input_text) - 1] = '\0';
			current_key = '\0';
			current_char_index = 0;
			LCD_ClearLine(1);
		}
	} else {
		int map_index = -1;
		for (int i = 0; i < sizeof(t9key_map) / sizeof(KeyMap); ++i) {
			if (t9key_map[i].key == key) {
				map_index = i;
				break;
			}
		}

		if (map_index == -1) return;

		if (key == current_key && (current_time - last_key_time < 500)) {
			// Same key pressed within timeout period, cycle to the next character
			current_char_index = (current_char_index + 1) % strlen(t9key_map[map_index].chars);
			input_text[strlen(input_text) - 1] = t9key_map[map_index].chars[current_char_index]; // Replace last char
		} else {
			// Different key pressed or timeout period exceeded, move to the next character
			if (strlen(input_text) < 16) {
				current_key = key;
				current_char_index = 0;
				strncat(input_text, &t9key_map[map_index].chars[current_char_index], 1);
			}
		}

		last_key_time = current_time;
	}

	LCD_SetCursor(1, 0);
	LCD_SendString(input_text);
}
