/*
 * stopwatch.h
 *
 *  Created on: Jul 27, 2024
 *      Author: zekro
 */

#ifndef INC_STOPWATCH_H_
#define INC_STOPWATCH_H_
void updateLCD(uint32_t count);
void EnterStopwatch();
void LCD_Reset();
//void toggleStopwatch(void);
//void resetStopwatch(void);
extern volatile uint32_t milliseconds;

#endif /* INC_STOPWATCH_H_ */
