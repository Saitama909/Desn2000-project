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

volatile int timer_playing = 0;
extern volatile DeviceState deviceState;
extern volatile DeviceState prevState;
volatile TimerMode currentTimer = {0};
volatile TimerMode previousTimer = {0};

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

		if (deviceState.mainMode != previousState.mainMode) {
			return;
		}

		if (hasTimerChanged(deviceState.timerMode)) {
			display_timer(deviceState.timerMode);
		}
	}
}

void start_timer(int timer_index) {
	// if none of them are running
	if (!user.timers[TIMER1].running && !user.timers[TIMER2].running && !user.timers[TIMER3].running && !user.timers[TIMER4].running) {
		HAL_TIM_Base_Start_IT(&htim7);
	}

	user.timers[timer_index].running = 1;

	while (user.timers[timer_index].running) {
		if (deviceState.timerMode == timer_index && deviceState.mainMode != TIMER_MODE) {
			update_time(user.timers[timer_index].remaining_time);
		}

		if (user.timers[timer_index].remaining_time == 0) {
			if (deviceState.timerMode == timer_index) {
				update_time(user.timers[timer_index].remaining_time);
			}

			stop_timer(timer_index);
			timer_playing = 1;
			play_timer_alert(timer_index);
		}
	}
}

void stop_timer(int timer_index) {
	user.timers[timer_index].running = 0;

	if (!user.timers[TIMER1].running && !user.timers[TIMER2].running && !user.timers[TIMER3].running && !user.timers[TIMER4].running) {
		HAL_TIM_Base_Stop_IT(&htim7);
	}
}

void play_timer_alert(int timer_index) {
	while (timer_playing) {
		play_alert(&user.timers[timer_index].alert);
	}
	TIM1->CCR3 = 0;
	user.timers[timer_index].remaining_time = user.timers[timer_index].duration;

	if (deviceState.timerMode == timer_index) {
		update_time(user.timers[timer_index].remaining_time);
	}
}
