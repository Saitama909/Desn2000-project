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
		.timers = 0
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

// ONLY CALLED WHEN STATE = CHOOSE_TIMER_COUNT
int choose_timer_count() {
	char num_timers = '\0';

	while (1) {
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
					return num_timers;
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

//void config_timer_duration(int num_timers) {
//	if (key && state == CONFIGURE_TIMER_DURATION) {
//		LCD_Clear();
//		LCD_SetCursor(0, 0);
//		for (int i = 1; i <= num_timers; i++) {
//			char buffer[17] = "";
//			snprintf(buffer, sizeof(buffer), "Timer %d duration", i);
//			LCD_SendString(buffer);
//			LCD_SetCursor(1, 0);
//			// enter_timer_duration();
//		}
//	}
//}
//
//void display_time(int hours, int mins, int secs) {
//	char buffer[9];
//	snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, mins, secs);
//	LCD_SendString(buffer);
//}
