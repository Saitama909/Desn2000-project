/*
 * timer.h
 *
 *  Created on: Jul 29, 2024
 *      Author: Anna Yang
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

extern volatile int timer_playing;
extern volatile TimerMode currentTimer;
extern volatile TimerMode previousTimer;

bool hasTimerChanged(TimerMode currentTimer);
void init_timers();
void display_timer(TimerMode timer);
void update_time(int input_secs);
void EnterTimer();
void start_timer(int timer_index);
void stop_timer(int timer_index);
void play_timer_alert(int timer_index);

#endif /* INC_TIMER_H_ */
