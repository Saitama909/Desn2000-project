/*
 * lcd_keypad.c
 *
 *  Created on: Jul 26, 2024
 *      Author: Anna Yang
 */
#include "lcd_keypad.h"

void LCD_Pulse() {
    HAL_GPIO_WritePin(E_PORT, LCD_E_PIN, GPIO_PIN_SET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(E_PORT, LCD_E_PIN, GPIO_PIN_RESET);
    HAL_Delay(5);
}

void LCD_PutNibble(uint8_t nibble) {
    HAL_GPIO_WritePin(D_PORT, LCD_D4_PIN, (nibble & 0x01));
    HAL_GPIO_WritePin(D_PORT, LCD_D5_PIN, ((nibble >> 1) & 0x01));
    HAL_GPIO_WritePin(D_PORT, LCD_D6_PIN, ((nibble >> 2) & 0x01));
    HAL_GPIO_WritePin(D_PORT, LCD_D7_PIN, ((nibble >> 3) & 0x01));
}

void LCD_Data(uint8_t c) {
    HAL_GPIO_WritePin(RS_PORT, LCD_RS_PIN, GPIO_PIN_SET);
    LCD_PutNibble(c >> 4);
    LCD_Pulse();
    LCD_PutNibble(c & 0x0F);
    LCD_Pulse();
    HAL_GPIO_WritePin(RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
}

void LCD_Command(uint8_t c) {
    HAL_GPIO_WritePin(RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    LCD_PutNibble(c >> 4);
    LCD_Pulse();
    LCD_PutNibble(c & 0x0F);
    LCD_Pulse();
}

void init_magic() {
    HAL_Delay(50);
    LCD_PutNibble(0x03);
    LCD_Pulse();
    HAL_Delay(5);
    LCD_PutNibble(0x03);
    LCD_Pulse();
    HAL_Delay(1);
    LCD_PutNibble(0x03);
    LCD_Pulse();
    LCD_PutNibble(0x02);
    LCD_Pulse();
    LCD_Command(0x28);
    LCD_Command(0x08);
    LCD_Command(0x01);
    LCD_Command(0x06);
    LCD_Command(0x0C);
}

void LCD_SendString(char* str) {
    while (*str) {
        LCD_Data(*str++);
    }
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x00 : 0x40;
    address += col;
    LCD_Command(0x80 | address);
}

void LCD_Clear(void) {
    LCD_Command(0x01);
    HAL_Delay(5);
}

uint8_t scan_keypad() {
    uint8_t ret = '\0';
    for (int c = 0; c < 4; c++) {
        HAL_GPIO_WritePin(C_PORT, colpins[c], GPIO_PIN_SET);
        for (int r = 0; r < 4; r++) {
            if (HAL_GPIO_ReadPin(R_PORT, rowpins[r])) {
                while (HAL_GPIO_ReadPin(R_PORT, rowpins[r])) {
                }
                HAL_Delay(50);
                ret = key_map[r][c];
            }
        }
        HAL_GPIO_WritePin(C_PORT, colpins[c], GPIO_PIN_RESET);
    }
    return ret;
}
