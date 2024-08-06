/*
 * alarm.h
 *
 *  Created on: Jul 28, 2024
 *      Author: kaira
 */

#ifndef INC_ALARM_H_
#define INC_ALARM_H_

void SetAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds);
void EnableAlarm(uint8_t enable);
void SnoozeAlarm(void);
void StopAlarm(void);
void DisplayAlarm();
void GetUserAlarmInput(void);

#endif /* INC_ALARM_H_ */
