/*
 * lcd.c
 *
 *  Created on: Jun 15, 2025
 *      Author: theis
 */

// lcd.c
#include "lcd.h"
#include "main.h"
#include "fixp.h"

#define RS_PORT GPIOA
#define RS_PIN  GPIO_PIN_0
#define EN_PORT GPIOA
#define EN_PIN  GPIO_PIN_1
#define D4_PORT GPIOA
#define D4_PIN  GPIO_PIN_4
#define D5_PORT GPIOA
#define D5_PIN  GPIO_PIN_5
#define D6_PORT GPIOA
#define D6_PIN  GPIO_PIN_6
#define D7_PORT GPIOB
#define D7_PIN  GPIO_PIN_3

static void LCD_EnablePulse(void) {
    HAL_GPIO_WritePin(EN_PORT, EN_PIN, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(EN_PORT, EN_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);
}

static void LCD_Send4Bits(uint8_t data) {
    HAL_GPIO_WritePin(D4_PORT, D4_PIN, (data >> 0) & 0x01);
    HAL_GPIO_WritePin(D5_PORT, D5_PIN, (data >> 1) & 0x01);
    HAL_GPIO_WritePin(D6_PORT, D6_PIN, (data >> 2) & 0x01);
    HAL_GPIO_WritePin(D7_PORT, D7_PIN, (data >> 3) & 0x01);
    LCD_EnablePulse();
}

void LCD_SendCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(RS_PORT, RS_PIN, GPIO_PIN_RESET);
    LCD_Send4Bits(cmd >> 4);
    LCD_Send4Bits(cmd & 0x0F);
    HAL_Delay(2);
}

void LCD_SendChar(char c) {
    HAL_GPIO_WritePin(RS_PORT, RS_PIN, GPIO_PIN_SET);
    LCD_Send4Bits(c >> 4);
    LCD_Send4Bits(c & 0x0F);
    HAL_Delay(1);
}

void LCD_SendString(char* str) {
    while (*str) {
        LCD_SendChar(*str++);
    }
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0 ? 0x00 : 0x40) + col;
    LCD_SendCommand(0x80 | addr);
}

void LCD_Init(void) {
    HAL_Delay(40);

    LCD_Send4Bits(0x03); HAL_Delay(5);
    LCD_Send4Bits(0x03); HAL_Delay(1);
    LCD_Send4Bits(0x03); HAL_Delay(1);
    LCD_Send4Bits(0x02); // Set to 4-bit mode

    LCD_SendCommand(0x28); // Function set: 4-bit, 2 line, 5x8 dots
    LCD_SendCommand(0x0C); // Display ON, Cursor OFF
    LCD_SendCommand(0x06); // Entry mode: Move cursor right
    LCD_SendCommand(0x01); // Clear display
    HAL_Delay(2);
}

void LCD_PrintInt(int32_t value) {
    if (value == 0) {
        LCD_SendString("0    ");
        return;
    }
    char buffer[12];
    sprintf(buffer, "%ld  ", (long)value);
    LCD_SendString(buffer);
}

void LCD_PrintAngle(int32_t angle) {
    //angle = inconvert(angle);  // Convert to integer degrees

    if (angle < 0) {
        LCD_SendChar('-');
        angle = -angle;
    }
    LCD_PrintInt(angle);
}

