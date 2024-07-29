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

void init_timers() {
    for (int i = 0; i < user.num_timers; i++) {
        user.timers[i].remaining_time = user.timers[i].duration;
        user.timers[i].running = 0;
    }
}

//void start_stop_timer() {
//	int timer_index = deviceState.timerMode;
//	if (user.timers[timer_index].running) {
//		// Stop the timer
//		user.timers[timer_index].running = 0;
//	} else {
//		// Start the timer
//		user.timers[timer_index].running = 1;
//		start_timer_countdown(timer_index);
//	}
//}
