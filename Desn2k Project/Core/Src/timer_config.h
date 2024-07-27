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
	enum State state;
	int timers;
} User;

void welcome();
int choose_timer_count();


#endif /* SRC_TIMER_CONFIG_H_ */
