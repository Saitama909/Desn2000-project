/*
 * lcd_keypad.c
 *
 *  Created on: Jul 26, 2024
 *      Author: Anna Yang
 */
#include "lcd_keypad.h"

uint8_t key_map[4][4] = {{'1','2','3','A'},
                         {'4','5','6','B'},
                         {'7','8','9','C'},
                         {'*','0','#','D'}};
uint16_t colpins[4] = {C1_PIN, C2_PIN, C3_PIN, C4_PIN};
uint16_t rowpins[4] = {R1_PIN, R2_PIN, R3_PIN, R4_PIN};

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

void LCD_ClearLine(int line) {
	LCD_SetCursor(line, 0);
	for (int i = 0; i < 16; i++) {
		LCD_SendString(" ");
	}
}

uint8_t scan_keypad() {
    static uint32_t last_scan_time = 0;
    uint8_t ret = '\0';
    uint32_t current_time = HAL_GetTick(); // Get the current time in milliseconds

    // Only perform the scan if enough time has passed since the last scan
    if (current_time - last_scan_time >= 50) {
        for (int c = 0; c < 4; c++) {
            // Set the current column to high
            HAL_GPIO_WritePin(C_PORT, colpins[c], GPIO_PIN_SET);

            for (int r = 0; r < 4; r++) {
                // Check if the key in the current row and column is pressed
                if (HAL_GPIO_ReadPin(R_PORT, rowpins[r]) == GPIO_PIN_SET) {
                    // Wait for the key to be released
                    while (HAL_GPIO_ReadPin(R_PORT, rowpins[r]) == GPIO_PIN_SET) {
                    }
                    // Update the return value with the key pressed
                    ret = key_map[r][c];
                    HAL_GPIO_WritePin(C_PORT, colpins[c], GPIO_PIN_RESET);
                    break; // Exit the row loop as a key has been detected
                }
            }

            // Reset the current column
            HAL_GPIO_WritePin(C_PORT, colpins[c], GPIO_PIN_RESET);
        }

        // Update the last scan time
        last_scan_time = current_time;
    }

    return ret;
}
