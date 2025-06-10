#ifndef _FIXP_H_
#define _FIXP_H_

/* Includes -------------------------------------------------------------*/
#include <stdint.h> // Whatever needs to be included
#include <stdio.h>

/* Exported types -------------------------------------------------------*/
/* Exported constants ---------------------------------------------------*/
/* Exported macro -------------------------------------------------------*/

// #define // Whatever needs to be defined

#define FIX14_SHIFT 14
#define FIX14_MULT(a, b) ( (a)*(b) >> FIX14_SHIFT )
#define FIX14_DIV(a, b)  ( ((a) << FIX14_SHIFT) / b )

#define FIX16_SHIFT 16
#define FIX16_MULT(a, b) ( (int32_t)(((int64_t)(a) * (int64_t)(b)) >> FIX16_SHIFT) ) // No change; keep intermediate 64-bit multiplication.
#define FIX16_DIV(a, b)  ( (int32_t)(((int64_t)(a) << FIX16_SHIFT) / (int64_t)(b)) ) // Still uses 64-bit intermediate.


/* Exported functions -------------------------------------------------- */

int32_t Rational(int a, int b);
void printFix(int32_t i); // Prints a signed 16.16 fixed point number
int32_t squareroot(uint32_t x); // Returns the square root of a 16.16 fixed point number
int32_t expand(int32_t i); // Converts an 18.14 fixed point number to 16.16
int32_t convert(int32_t i); // converts a 32.0 fixed point number to 16.16
int32_t inconvert(int32_t i); // converts a 16.16 fixed point number to 32.0

#endif /* _FIXP_H_ */
