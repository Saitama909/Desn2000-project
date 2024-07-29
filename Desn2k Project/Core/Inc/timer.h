/*
 * timer.h
 *
 *  Created on: Jul 29, 2024
 *      Author: Anna Yang
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

void init_timers();
void display_timer();
void EnterTimer();
void start_stop_timer();
void start_timer(int timer_index);
void stop_timer(int timer_index);
void play_timer_alert(int timer_index);

#endif /* INC_TIMER_H_ */
