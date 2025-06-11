/*
 * lsm9ds1.c
 *
 *  Created on: 10. jun. 2025
 *      Author: theis
 */

#include "lsm9ds1.h"
#include "fixp.h"

void lsmCtrlReg(I2C_HandleTypeDef*handle) {
	// Enable accelerometer: ODR = 119 Hz, ±2g, BW = 50 Hz

	  uint8_t acc_ctrl_reg8[2] = {0x22, 0x05}; // reboot + soft reset
	  HAL_I2C_Master_Transmit(handle, LSM9DS1_ACC_ADDRESS << 1, acc_ctrl_reg8, 2, HAL_MAX_DELAY);
	  HAL_Delay(100);

	  uint8_t acc_ctrl_reg6[2] = {0x20, 0x60}; // 0x60 = 0b01100000 → ODR = 119Hz, ±2g
	  HAL_I2C_Master_Transmit(handle, LSM9DS1_ACC_ADDRESS << 1, acc_ctrl_reg6, 2, HAL_MAX_DELAY);
	  HAL_Delay(10);

	  uint8_t mag_ctrl_reg1[2] = {0x20, 0b01110000}; //
	  HAL_I2C_Master_Transmit(handle, LSM9DS1_MAG_ADDRESS << 1, mag_ctrl_reg1, 2, HAL_MAX_DELAY);
	  HAL_Delay(10);

	  uint8_t mag_ctrl_reg3[2] = {0x22, 0b00000000}; //
	  HAL_I2C_Master_Transmit(handle, LSM9DS1_MAG_ADDRESS << 1, mag_ctrl_reg3, 2, HAL_MAX_DELAY);
	  HAL_Delay(10);

	  uint8_t mag_ctrl_reg4[2] = {0x23, 0b00001100}; //
	  HAL_I2C_Master_Transmit(handle, LSM9DS1_MAG_ADDRESS << 1, mag_ctrl_reg4, 2, HAL_MAX_DELAY);
	  HAL_Delay(10);
}

Vector3D lsmMagOut(I2C_HandleTypeDef*handle) {
	/*
	 * returns:
	 */
	Vector3D raw = lsmMagRead(handle); // comes out in signed 32-bit, ranging from 0 to 32767
	Vector3D out = {
			Rational(raw.x << 1, 32768), // -2 to 2 in fixp
			Rational(raw.y << 1, 32768), // -2 to 2 in fixp
			Rational(raw.z << 1, 32768) // -2 to 2 in fixp
	};

	Matrix3x3 calA = create_matrix(
			Rational(9785,10000), Rational(597,10000), Rational(78,10000),
			Rational(597,10000), Rational(10562,10000), Rational(-111, 10000),
			Rational(78,10000), Rational(-111,10000), Rational(9712,10000)
	);
	Vector3D calb = {
			Rational(1110,10000),
			Rational(-1736,10000),
			Rational(1973, 10000),
	};

	return out;
}

Vector3D lsmAccRead(I2C_HandleTypeDef*handle) {
	// Magnetometer example code
	/*uint16_t buffer;
	HAL_I2C_Master_Transmit(&hi2c3, LSM9DS1_MAG_ADDRESS << 1, OUT_X_L_M, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c3, LSM9DS1_MAG_ADDRESS << 1 | 0x01, buffer, sizeof(buffer), HAL_MAX_DELAY);
	printf("%d\n", buffer);
	*/

	uint8_t reg1 = OUT_X_XL | 0x80; // Enable auto-increment
	uint8_t data1[6];

	// Request starting from OUT_X_XL with auto-increment
	HAL_I2C_Master_Transmit(handle, LSM9DS1_ACC_ADDRESS << 1, &reg1, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(handle, (LSM9DS1_ACC_ADDRESS << 1) | 0x01, data1, 6, HAL_MAX_DELAY);

	// Combine bytes into signed 16-bit integers
	int16_t acc_x = (int16_t)(data1[1] << 8 | data1[0]);
	int16_t acc_y = (int16_t)(data1[3] << 8 | data1[2]);
	int16_t acc_z = (int16_t)(data1[5] << 8 | data1[4]);

	Vector3D output = {acc_x, acc_y, acc_z};
	return output;
}

Vector3D lsmMagRead(I2C_HandleTypeDef*handle) {
	uint8_t reg2 = OUT_X_L_M | 0x80; // Enable auto-increment
	uint8_t data2[6];

	// Request starting from OUT_X_XL with auto-increment
	HAL_I2C_Master_Transmit(handle, LSM9DS1_MAG_ADDRESS << 1, &reg2, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(handle, (LSM9DS1_MAG_ADDRESS << 1) | 0x01, data2, 6, HAL_MAX_DELAY);

	// Combine bytes into signed 16-bit integers
	int16_t mag_x = (int16_t)(data2[1] << 8 | data2[0]);
	int16_t mag_y = (int16_t)(data2[3] << 8 | data2[2]);
	int16_t mag_z = (int16_t)(data2[5] << 8 | data2[4]);

	Vector3D output = {mag_x, mag_y, mag_z};
	return output;
}


/*
void temprasmus() {
	  /////////////////////////////////////////////////////// HER LAVER RASMUS KÆMPE YUM YUM //////////////////////////////////////////////////////////////////////////////////
	  // Static arrays to store [min, max] for each axis
	  static int16_t mag_x_minmax[2] = {INT16_MAX, INT16_MIN};
	  static int16_t mag_y_minmax[2] = {INT16_MAX, INT16_MIN};
	  static int16_t mag_z_minmax[2] = {INT16_MAX, INT16_MIN};

	  // Determine if new max or min for each axis
	  int16_t CODE_X = 0;
	  if (mag_x > mag_x_minmax[1]) {
	      mag_x_minmax[1] = mag_x; // update max
	      CODE_X = 10;
	  } else if (mag_x < mag_x_minmax[0]) {
	      mag_x_minmax[0] = mag_x; // update min
	      CODE_X = 1;
	  } else {
	      CODE_X = 0;
	  }

	  int16_t CODE_Y = 0;
	  if (mag_y > mag_y_minmax[1]) {
	      mag_y_minmax[1] = mag_y; // update max
	      CODE_Y = 10;
	  } else if (mag_y < mag_y_minmax[0]) {
	      mag_y_minmax[0] = mag_y; // update min
	      CODE_Y = 1;
	  } else {
	      CODE_Y = 0;
	  }

	  int16_t CODE_Z = 0;
	  if (mag_z > mag_z_minmax[1]) {
	      mag_z_minmax[1] = mag_z; // update max
	      CODE_Z = 10;
	  } else if (mag_z < mag_z_minmax[0]) {
	      mag_z_minmax[0] = mag_z; // update min
	      CODE_Z = 1;
	  } else {
	      CODE_Z = 0;
	  }
}
*/
