/*
 * lcd_keypad.h
 *
 *  Created on: Jul 26, 2024
 *      Author: Anna Yang
 */

#ifndef SRC_LCD_KEYPAD_H_
#define SRC_LCD_KEYPAD_H_

#define D_PORT GPIOC
#define LCD_D4_PIN GPIO_PIN_8
#define LCD_D5_PIN GPIO_PIN_9
#define LCD_D6_PIN GPIO_PIN_10
#define LCD_D7_PIN GPIO_PIN_11
#define E_PORT GPIOD
#define RW_PORT GPIOC
#define RS_PORT GPIOA
#define LCD_E_PIN GPIO_PIN_2
#define LCD_RW_PIN GPIO_PIN_6
#define LCD_RS_PIN GPIO_PIN_15

#define C_PORT GPIOA
#define R_PORT GPIOB

#define C1_PIN GPIO_PIN_8
#define C2_PIN GPIO_PIN_9
#define C3_PIN GPIO_PIN_10
#define C4_PIN GPIO_PIN_11

#define R1_PIN GPIO_PIN_11
#define R2_PIN GPIO_PIN_12
#define R3_PIN GPIO_PIN_13
#define R4_PIN GPIO_PIN_14

#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern uint8_t key_map[4][4];
extern uint16_t colpins[4];
extern uint16_t rowpins[4];

void init_magic();
void LCD_Pulse();
void LCD_PutNibble(uint8_t nibble);
void LCD_Data(uint8_t c);
void LCD_Command(uint8_t c);
void LCD_SendString(char* str);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Clear(void);
void LCD_ClearLine(int line);
uint8_t scan_keypad();

#endif /* SRC_LCD_KEYPAD_H_ */
