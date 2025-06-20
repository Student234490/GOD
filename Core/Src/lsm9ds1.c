/*
 * lsm9ds1.c
 *
 *  Created on: 10. jun. 2025
 *      Author: theis
 */

#include "lsm9ds1.h"
#include "fixp.h"
#include "main.h"
#include "vector.h"

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

/* --------------------------------------------------------------------------
 *  readSensorsAndAverage() – returns the mean of the last 5 samples
 * --------------------------------------------------------------------------*/
#define AVG_LEN 3                 /* running-mean window length */

static Vector3D acc_buf[AVG_LEN]; /* FIFO for accelerometer */
static Vector3D mag_buf[AVG_LEN]; /* FIFO for magnetometer  */

static uint8_t  buf_pos    = 0;   /* write index 0…AVG_LEN-1 */
static uint8_t  buf_filled = 0;   /* number of samples in FIFO */

static int64_t  acc_sum_x = 0, acc_sum_y = 0, acc_sum_z = 0;
static int64_t  mag_sum_x = 0, mag_sum_y = 0, mag_sum_z = 0;

void readSensorsAndAverage(Vector3D* acc_avg, Vector3D* mag_avg, I2C_HandleTypeDef hi2c3)
{
	Matrix3x3 softIron = create_matrix(
			57855,   6259, -1016,     /* row 0 */
			6259,    73662, 813,     /* row 1 */
			-1016,   813, 66696      /* row 2 */
	);
    /* 1. fresh raw sensor counts --------------------------------------- */
    Vector3D acc_raw = lsmAccRead(&hi2c3);     /* ±16 384 cnt ≈ 1 g   */
    Vector3D mraw    = lsmMagRead(&hi2c3);     /* ±32 768 cnt        */

    /* 2. axis-remap + hard-iron offsets (still in raw counts) ---------- */
    Vector3D mag_raw;
    /*
    mag_raw.x =  mraw.y + 2637; //+ 2897;    +Ymag → +Xbody
    mag_raw.y = -mraw.x + 3352;//2486.0; //+ 3352;    –Xmag → +Ybody
    mag_raw.z =  mraw.z + 3376; //+ 3200;    Zmag →  Zbody
    */

	mag_raw.x =  convert(mraw.x) - convert(2832 + 54); //+ 2897;   /* +Ymag → +Xbody */
    mag_raw.y =  convert(mraw.y) + convert(3259 - 192); //2486.0; //+ 3352;   /* –Xmag → +Ybody */
	mag_raw.z =  convert(mraw.z) + convert(4243 - 80); //+ 3200;   /*  Zmag →  Zbody */

	mag_raw = MVMult(softIron, mag_raw);

	/* 5. push *m_soft* into the FIFO instead of the raw reading ----------- */

    acc_raw.x = acc_raw.x + 376;
    acc_raw.y = acc_raw.y + 282 + 133;
    acc_raw.z = acc_raw.z - 738;

    /* 3. promote to Q16.16  (raw × 4  ==  << 2) ------------------------ */
    Vector3D acc_q16 = { acc_raw.x,
                         acc_raw.y,
                         acc_raw.z};

    Vector3D mag_q16 = { mag_raw.x>>16,
                         mag_raw.y>>16,
                         mag_raw.z>>16};

    /* 4. drop oldest sample if FIFO full ------------------------------ */
    if (buf_filled == AVG_LEN) {
        acc_sum_x -= acc_buf[buf_pos].x;
        acc_sum_y -= acc_buf[buf_pos].y;
        acc_sum_z -= acc_buf[buf_pos].z;

        mag_sum_x -= mag_buf[buf_pos].x;
        mag_sum_y -= mag_buf[buf_pos].y;
        mag_sum_z -= mag_buf[buf_pos].z;
    }

    /* 5. store new sample & update sums -------------------------------- */
    acc_buf[buf_pos] = acc_q16;
    mag_buf[buf_pos] = mag_q16;

    acc_sum_x += acc_q16.x;  acc_sum_y += acc_q16.y;  acc_sum_z += acc_q16.z;
    mag_sum_x += mag_q16.x;  mag_sum_y += mag_q16.y;  mag_sum_z += mag_q16.z;

    /* 6. advance circular index --------------------------------------- */
    buf_pos = (buf_pos + 1) % AVG_LEN;
    if (buf_filled < AVG_LEN) buf_filled++;

    /* 7. return running mean ------------------------------------------ */
    int32_t div = buf_filled;           /* 1 … 5 */

    acc_avg->x = (int32_t)(acc_sum_x / div);
    acc_avg->y = (int32_t)(acc_sum_y / div);
    acc_avg->z = (int32_t)(acc_sum_z / div);

    mag_avg->x = (int32_t)(mag_sum_x / div);
    mag_avg->y = (int32_t)(mag_sum_y / div);
    mag_avg->z = (int32_t)(mag_sum_z / div);
}

