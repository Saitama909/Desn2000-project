/*
 * timer.c
 *
 *  Created on: Jul 29, 2024
 *      Author: Anna Yang
 */

#include "main.h"
#include "lcd_keypad.h"
#include "timer_config.h"
#include "timer.h"

extern volatile int note_playing;
volatile int timer_playing = 0;
extern volatile DeviceState deviceState;
extern volatile DeviceState prevState;
volatile TimerMode currentTimer;
volatile TimerMode previousTimer;
volatile int playFinishedAlert[4] = {0};

bool hasTimerChanged(TimerMode currentTimer) {
    bool changed = false;

    if (currentTimer != previousTimer) {
        changed = true;
    }

    // Update previousTimer to the current state
    previousTimer = currentTimer;

    return changed;
}

void init_timers() {
    for (int i = 0; i < user.num_timers; i++) {
        user.timers[i].remaining_time = user.timers[i].duration;
        user.timers[i].running = 0;
    }
}

void display_timer(TimerMode timer) {
	if (deviceState.mainMode == TIMER_MODE) {
		LCD_Clear();
		LCD_SetCursor(0, 0);
		char buffer[4] = "";
		snprintf(buffer, sizeof(buffer), "%d: ", timer + 1);
		LCD_SendString(buffer);
		LCD_SendString(user.timers[timer].name);

		update_time(user.timers[timer].remaining_time);
	}
}

void update_time(int input_secs) {
	int hours = input_secs / 3600;
	int mins = (input_secs % 3600) / 60;
	int secs = input_secs % 60;

	char buffer[22] = "";
	snprintf(buffer, sizeof(buffer), "%01d:%02d:%02d", hours, mins, secs);

	LCD_SetCursor(1, 0);
	LCD_SendString(buffer);
}

void EnterTimer() {
	previousTimer = deviceState.timerMode;
	display_timer(currentTimer);
	while(1) {
		char input = scan_keypad();
		if (input == 'A') {
			start_timer(TIMER1);
		} else if (input == 'B') {
			start_timer(TIMER2);
		} else if (input == 'C') {
			start_timer(TIMER3);
		} else if (input == 'D') {
			start_timer(TIMER4);
		}

		// check if switched to clock mode
		if (deviceState.mainMode != TIMER_MODE) {
			return;
		}

		// check if switched to different timer
		if (hasTimerChanged(deviceState.timerMode)) {
			display_timer(deviceState.timerMode);
		}
		// if focused on timer and timer running
		else if (user.timers[deviceState.timerMode].running) {
			update_time(user.timers[deviceState.timerMode].remaining_time);
		}
		// check if any timer has ended
		int timerEnded = -1;
		if ((timerEnded = checkTimerEnded()) != -1) {
			stop_timer(timerEnded);
			if (timerEnded == deviceState.timerMode) {
				update_time(user.timers[timerEnded].remaining_time);
			}
			timer_playing = 1;
			play_timer_alert(timerEnded);
			user.timers[timerEnded].remaining_time = user.timers[timerEnded].duration;
			playFinishedAlert[timerEnded] = 0;
		}
	}
}

void start_timer(int timer_index) {
	// if none of them are running
	if (!user.timers[TIMER1].running && !user.timers[TIMER2].running && !user.timers[TIMER3].running && !user.timers[TIMER4].running) {
		HAL_TIM_Base_Start_IT(&htim7);
	}

	// set to running
	user.timers[timer_index].running = 1;

//	while (user.timers[timer_index].running) {
//		if (deviceState.timerMode == timer_index && deviceState.mainMode != TIMER_MODE) {
//			update_time(user.timers[timer_index].remaining_time);
//		}
//
//		if (user.timers[timer_index].remaining_time == 0) {
//			if (deviceState.timerMode == timer_index) {
//				update_time(user.timers[timer_index].remaining_time);
//			}
//
//			stop_timer(timer_index);
//			timer_playing = 1;
//			play_timer_alert(timer_index);
//		}
//	}
}

void stop_timer(int timer_index) {
	user.timers[timer_index].running = 0;

	if (!user.timers[TIMER1].running && !user.timers[TIMER2].running && !user.timers[TIMER3].running && !user.timers[TIMER4].running) {
		HAL_TIM_Base_Stop_IT(&htim7);
	}
}

void play_timer_alert(int timer_index) {
	for (int i = 0; i < user.timers[timer_index].alert.num_notes; i++) {
		Note note = user.timers[timer_index].alert.notes[i];
		TIM1->ARR = (72000000 / (note.freq * 1000)) - 1;
		TIM1->CCR3 = TIM1->ARR / 4;

		// NOTE: WE DO NOT WANT TO USE DELAYS - USE ANOTHER TIMER INSTEAD FOR THE DURATION OF THE NOTE
		__HAL_TIM_SET_AUTORELOAD(&htim16, note.duration * 10 - 1);
		__HAL_TIM_CLEAR_FLAG(&htim16, TIM_FLAG_UPDATE);

		HAL_TIM_Base_Start_IT(&htim16);
		note_playing = 1;
		while (note_playing) {
			if (timer_playing == 0) {
				HAL_TIM_Base_Stop_IT(&htim16);
				TIM1->CCR3 = 0;
				note_playing = 0;
				return;
			}
		}
	}

	TIM1->CCR3 = 0;
	timer_playing = 0;
	user.timers[timer_index].remaining_time = user.timers[timer_index].duration;
}

//void play_alert(volatile Song *song) {
//    for (int i = 0; i < song->num_notes; i++) {
//        Note note = song->notes[i];
//        TIM1->ARR = (72000000 / (note.freq * 1000)) - 1;
//        TIM1->CCR3 = TIM1->ARR / 4;
//
//        // NOTE: WE DO NOT WANT TO USE DELAYS - USE ANOTHER TIMER INSTEAD FOR THE DURATION OF THE NOTE
//        __HAL_TIM_SET_AUTORELOAD(&htim16, note.duration * 10 - 1);
//        __HAL_TIM_CLEAR_FLAG(&htim16, TIM_FLAG_UPDATE);
//
//        HAL_TIM_Base_Start_IT(&htim16);
//        note_playing = 1;
//        while (note_playing) {
//        	if (timer_playing == 0) {
//				HAL_TIM_Base_Stop_IT(&htim16);
//				TIM1->CCR3 = 0;
//				note_playing = 0;
//				return;
//			}
//        }
//    }
//    TIM1->CCR3 = 0;
//    timer_playing = 0;
//}

int checkTimerEnded() {
	if (playFinishedAlert[TIMER1]) {
		return TIMER1;
	}

	if (playFinishedAlert[TIMER2]) {
		return TIMER2;
	}

	if (playFinishedAlert[TIMER3]) {
		return TIMER3;
	}

	if (playFinishedAlert[TIMER4]) {
		return TIMER4;
	}
	return -1;
}