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

void init_timers() {
    for (int i = 0; i < user.num_timers; i++) {
        user.timers[i].remaining_time = user.timers[i].duration;
        user.timers[i].running = 0;
    }
}

void display_timer() {
	if (deviceState.mainMode == TIMER_MODE) {
		LCD_SetCursor(0, 0);
		LCD_SendString(user.timers[deviceState.timerMode].name);

		update_time(user.timers[deviceState.timerMode].remaining_time);
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
	display_timer();

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

		if (hasStateChanged(deviceState)) {
			return;
		}
	}
}

void start_timer(int timer_index) {
	user.timers[timer_index].running = 1;

	switch (timer_index) {
		case TIMER1:
			HAL_TIM_Base_Start_IT(&htim7);
			break;
		case TIMER2:
			HAL_TIM_Base_Start_IT(&htim8);
			break;
		case TIMER3:
			HAL_TIM_Base_Start_IT(&htim2);
			break;
		case TIMER4:
			HAL_TIM_Base_Start_IT(&htim17);
			break;
	}

	while (user.timers[timer_index].running) {
		update_time(user.timers[timer_index].remaining_time);

		if (user.timers[timer_index].remaining_time == 0) {
			display_timer(user.timers[timer_index].remaining_time);
			stop_timer(timer_index);
			timer_playing = 1;
			play_timer_alert(timer_index);
		}
	}
}

void stop_timer(int timer_index) {
	user.timers[timer_index].running = 0;

	switch (timer_index) {
		case TIMER1:
			HAL_TIM_Base_Stop_IT(&htim7);
			break;
		case TIMER2:
			HAL_TIM_Base_Stop_IT(&htim8);
			break;
		case TIMER3:
			HAL_TIM_Base_Stop_IT(&htim2);
			break;
		case TIMER4:
			HAL_TIM_Base_Stop_IT(&htim17);
			break;
	}
}

void play_timer_alert(int timer_index) {
	while (timer_playing) {
		play_alert(&user.timers[timer_index].alert);
	}
	TIM1->CCR3 = 0;
	user.timers[timer_index].remaining_time = user.timers[timer_index].duration;
	update_time(user.timers[timer_index].remaining_time);
}
