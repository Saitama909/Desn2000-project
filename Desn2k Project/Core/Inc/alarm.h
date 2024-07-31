/*
 * alarm.h
 *
 *  Created on: Jul 28, 2024
 *      Author: kaira
 */

#ifndef INC_ALARM_H_
#define INC_ALARM_H_

void Set_Alarm(uint8_t hours, uint8_t minutes, uint8_t seconds);
void Enable_Alarm(uint8_t enable);
void Snooze_Alarm(void);
void Stop_Alarm(void);
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);
void Get_User_Alarm_Input(void);

#endif /* INC_ALARM_H_ */
