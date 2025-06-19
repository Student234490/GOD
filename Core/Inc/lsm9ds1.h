/*
 * lsm9ds1.h
 *
 *  Created on: 10. jun. 2025
 *      Author: theis
 */

#ifndef INC_LSM9DS1_H_
#define INC_LSM9DS1_H_

#include "stm32g4xx_hal.h"
#include "stdint.h"
#include "vector.h"

#define LSM9DS1_MAG_ADDRESS   0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_ACC_ADDRESS   0x6B

#define OUT_X_L_M   0x28 //
#define OUT_Y_L_M   0x2A //
#define OUT_Z_L_M   0x2C //

#define OUT_X_XL   0x28 //
#define OUT_Y_XL   0x2A //
#define OUT_Z_XL   0x2C //

void lsmCtrlReg(I2C_HandleTypeDef*handle);
Vector3D lsmMagRead(I2C_HandleTypeDef*handle);
Vector3D lsmAccRead(I2C_HandleTypeDef*handle);
Vector3D lsmMagOut(I2C_HandleTypeDef*handle);
void readSensorsAndAverage(Vector3D* acc_avg, Vector3D* mag_avg, I2C_HandleTypeDef hi2c3);

#endif /* INC_LSM9DS1_H_ */
