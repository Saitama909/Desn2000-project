/*
 * timer_config.c
 *
 *  Created on: Jul 26, 2024
 *      Author: Anna Yang
 */

#include "main.h"
#include "lcd_keypad.h"
#include "timer_config.h"


volatile User user = {
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

Song songs[6];

volatile int note_playing;

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
	// Cycle through specified number of timers
	for (int i = 0; i < user.num_timers; i++) {
		LCD_Clear();
		LCD_SetCursor(0, 0);

		user.state = CONFIGURE_TIMER_DURATION;
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

		user.state = CONFIGURE_TIMER_ALERT;
		LCD_Clear();
		LCD_SetCursor(0, 0);
		snprintf(buffer, sizeof(buffer), "Timer %d alert", i + 1);
		LCD_SendString(buffer);
		choose_timer_alert(i);
	}

	LCD_Clear();
	LCD_SetCursor(0, 0);
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
	snprintf(buffer, sizeof(buffer), "%01d:%02d:%02d", hours, mins, secs);

	LCD_SetCursor(1, 0);
	LCD_SendString(buffer);
}

void check_timer_duration(int input_secs, int timer_index) {
	int total_secs = input_secs;

	if (total_secs < 0) {
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
		user.timers[timer_index].duration = total_secs;
	}
}

void enter_timer_name(int timer_index) {
	LCD_SetCursor(1, 0);
	char input_text[17] = "";

	while (1) {
		char key = scan_keypad();
		if (key) {
			if (key == '#') {
				if (check_timer_name(timer_index, input_text)) {
					LCD_Clear();
					LCD_SetCursor(0, 0);
					LCD_SendString("Name already");
					LCD_SetCursor(1, 0);
					LCD_SendString("exists");
					HAL_Delay(1000);

					LCD_Clear();
					LCD_SetCursor(0, 0);
					char buffer[22] = "";
					snprintf(buffer, sizeof(buffer), "Timer %d name", timer_index + 1);
					LCD_SendString(buffer);

					memset(input_text, 0, 17);
				} else if (!strcmp(input_text, "")) {
					LCD_Clear();
					LCD_SetCursor(0, 0);
					LCD_SendString("You must provide");
					LCD_SetCursor(1, 0);
					LCD_SendString("a name");
					HAL_Delay(1000);

					LCD_Clear();
					LCD_SetCursor(0, 0);
					char buffer[22] = "";
					snprintf(buffer, sizeof(buffer), "Timer %d name", timer_index + 1);
					LCD_SendString(buffer);
				} else {
					strncpy(user.timers[timer_index].name, input_text, 16);
					user.timers[timer_index].name[16] = '\0';
					break;
				}
			} else {
				t9_typing(key, input_text);
			}
		}
	}

	LCD_Clear();
}

bool check_timer_name(int timer_index, char *input_text) {
	for (int i = 0; i < timer_index; i++) {
		if (!strcmp(input_text, user.timers[i].name)) {
			return true;
		}
	}
	return false;
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

void choose_timer_alert(int timer_index) {
    init_alerts();

    LCD_SetCursor(1, 0);
    LCD_SendString("Pick song 1-6: ");

    int selected_song = -1;

    while (1) {
		char key = scan_keypad();
		if (key) {
			int num = key - '0';
			if (num >= 1 && num <= 6) {
				selected_song = num - 1;
				LCD_SetCursor(1, 15);
				LCD_Data(key);
				play_alert(&songs[num - 1]);
			} else if (key == '#') {
				if (check_timer_alert(timer_index, selected_song)) {
					LCD_Clear();
					LCD_SetCursor(0, 0);
					LCD_SendString("Alert already");
					LCD_SetCursor(1, 0);
					LCD_SendString("exists");
					HAL_Delay(1000);

					LCD_Clear();
					LCD_SetCursor(0, 0);
					char buffer[22] = "";
					snprintf(buffer, sizeof(buffer), "Timer %d alert", timer_index + 1);
					LCD_SendString(buffer);
					LCD_SetCursor(1, 0);
					LCD_SendString("Pick song 1-6: ");

					selected_song = -1;
				} else if (selected_song == -1) {
					LCD_Clear();
					LCD_SetCursor(0, 0);
					LCD_SendString("You must select");
					LCD_SetCursor(1, 0);
					LCD_SendString("an alert");
					HAL_Delay(1000);

					LCD_Clear();
					LCD_SetCursor(0, 0);
					char buffer[22] = "";
					snprintf(buffer, sizeof(buffer), "Timer %d alert", timer_index + 1);
					LCD_SendString(buffer);
					LCD_SetCursor(1, 0);
					LCD_SendString("Pick song 1-6: ");
				} else {
					user.timers[timer_index].alert = songs[selected_song];
					break;
				}
			}
		}
	}
}

bool check_timer_alert(int timer_index, int selected_song) {
	for (int i = 0; i < timer_index; i++) {
		if (selected_song == user.timers[i].alert.id) {
			return true;
		}
	}
	return false;
}

void init_alerts() {
    Song c_maj_arp = {
        .notes = {
            {261, 500},
            {330, 500},
            {392, 500},
            {523, 750}
        },
        .num_notes = 4,
		.id = 0
    };
    songs[0] = c_maj_arp;

    Song windows_shutdown = {
        .notes = {
            {830, 500},
            {622, 500},
            {415, 500},
            {466, 500}
        },
        .num_notes = 4,
		.id = 1
    };
    songs[1] = windows_shutdown;

    Song annoying = {
        .notes = {
            {392, 200},
            {330, 200},
            {392, 200},
            {330, 200},
            {392, 200},
            {330, 200},
        },
        .num_notes = 6,
		.id = 2
    };
    songs[2] = annoying;

    Song amongus = {
        .notes = {
            {261, 200},
            {311, 200},
            {349, 200},
            {369, 200},
            {349, 200},
            {311, 200},
            {261, 600},
            {233, 100},
            {293, 100},
            {261, 600},
        },
        .num_notes = 10,
		.id = 3
    };
    songs[3] = amongus;

    Song doorbell = {
		.notes = {
			{329, 300},
			{261, 300},
			{293, 300},
			{196, 300},
			{196, 300},
			{293, 300},
			{329, 300},
			{261, 300},
		},
		.num_notes = 8,
		.id = 4
    };
    songs[4] = doorbell;

    Song c_maj_scale = {
		.notes = {
			{261, 200},
			{293, 200},
			{329, 200},
			{349, 200},
			{392, 200},
			{440, 200},
			{493, 200},
			{523, 200},
		},
		.num_notes = 8,
		.id = 5
	};
	songs[5] = c_maj_scale;
}

void play_alert(volatile Song *song) {
    for (int i = 0; i < song->num_notes; i++) {
        Note note = song->notes[i];
        TIM1->ARR = (72000000 / (note.freq * 1000)) - 1;
        TIM1->CCR3 = TIM1->ARR / 4;

        // NOTE: WE DO NOT WANT TO USE DELAYS - USE ANOTHER TIMER INSTEAD FOR THE DURATION OF THE NOTE
        __HAL_TIM_SET_AUTORELOAD(&htim16, note.duration * 10 - 1);
        __HAL_TIM_CLEAR_FLAG(&htim16, TIM_FLAG_UPDATE);

        HAL_TIM_Base_Start_IT(&htim16);
        note_playing = 1;
        while (note_playing) {

        }
    }
    TIM1->CCR3 = 0;
}
