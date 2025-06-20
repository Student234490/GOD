/*
 * lcd.h
 *
 *  Created on: Jun 15, 2025
 *      Author: theis
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32g4xx_hal.h"

// Initializes the LCD module
void LCD_Init(void);

// Sends a command byte to the LCD
void LCD_SendCommand(uint8_t cmd);

// Sends a single character to the LCD
void LCD_SendChar(char c);

// Sends a null-terminated string to the LCD
void LCD_SendString(char* str);

//prints int16_t on LCD
void LCD_PrintInt(int32_t value);
void LCD_PrintAngle(int32_t value);

// Moves the cursor to a specific row and column (0-indexed)
void LCD_SetCursor(uint8_t row, uint8_t col);

#endif // LCD_H
