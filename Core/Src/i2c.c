/*
 * i2c.c
 *
 *  Created on: 10. jun. 2025
 *      Author: theis
 */

#include <stdio.h>
#include <stdint.h>
#include "stm32g4xx_hal.h"

//
// code for testing I2C
//

void I2C_Scan(I2C_HandleTypeDef *hi2c) {
    HAL_StatusTypeDef result;
    uint8_t i;

    printf("Scanning I2C bus...\r\n");

    for (i = 1; i < 128; i++) {
        // Left-shift by 1 to form 8-bit address
        result = HAL_I2C_IsDeviceReady(hi2c, (i << 1), 1, 10);

        if (result == HAL_OK) {
            printf("I2C device found at address 0x%02X\r\n", i);
        }
    }

    printf("Scan complete.\r\n");
}
