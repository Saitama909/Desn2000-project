/*
 * stdtimer.h
 *
 *  Created on: Jul 30, 2024
 *      Author: zekro
 */

#ifndef INC_STDTIMER_H_
#define INC_STDTIMER_H_

extern volatile uint32_t time_left;
extern volatile int timerRunning;

void ConfigTimer();
void DisplayTimer();
void stdTimerAlert();

#endif /* INC_STDTIMER_H_ */
