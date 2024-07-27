/*
 * timer_config.h
 *
 *  Created on: Jul 26, 2024
 *      Author: Anna Yang
 */

#ifndef SRC_TIMER_CONFIG_H_
#define SRC_TIMER_CONFIG_H_

enum State {
	CONFIGURE_TIMER_COUNT,
	CONFIGURE_TIMER_DURATION,
	CONFIGURE_TIMER_NAME,
	CONFIGURE_TIMER_ALERT
};

typedef struct {
	int hours;
	int mins;
	int secs;
	char name[17];
} Timer;

typedef struct {
	enum State state;
	int num_timers;
	Timer timers[4];
} User;


void welcome();
void choose_timer_count();

void config_timer_duration();
void enter_timer_duration(int timer_index);
void check_timer_duration(int input_secs, int timer_index);
void display_time(int input_secs);


#endif /* SRC_TIMER_CONFIG_H_ */
