/*
 * luts.h
 *
 *  Created on: 6. jun. 2025
 *      Author: theis
 */

#ifndef _LUTS_H_
#define _LUTS_H_

/* Includes -------------------------------------------------------------*/
#include <stdint.h> // Whatever needs to be included
#include <stdio.h>
#include <fixp.h>

/* Exported types -------------------------------------------------------*/
/* Exported constants ---------------------------------------------------*/

/** Generated using Dr LUT - Free Lookup Table Generator
  * https://github.com/ppelikan/drlut
  **/
// Formula: sin(2*pi*t/T)

extern const uint16_t sinlut[1024];
extern const int32_t asin_lut[1024];
extern const int32_t atan_lut[1024];
/* Exported macro -------------------------------------------------------*/

// #define // Whatever needs to be defined

/* Exported functions -------------------------------------------------- */
int32_t sinus(int t); // takes integer representing a n-LUT degree, returns 16.16 decimal sine
int32_t cosinus(int t); // takes integer representing a n-LUT degree, returns 16.16 decimal cosine
int32_t sinrad(int32_t t); // takes integer representing a 16.16 radians, returns 16.16 decimal sine
int32_t cosrad(int32_t t); // takes integer representing a 16.16 radians, returns 16.16 decimal cosine

int32_t asin16(int32_t x_fix);
int32_t atan16(int32_t x_fix);
int32_t atan2_16(int32_t y, int32_t x);

int32_t fix_asin(int32_t x);
int32_t fix_atan2(int32_t y, int32_t x);

#endif /* INC_LUTS_H_ */
