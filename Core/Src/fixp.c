/*
 * fixp.c
 *
 *  Created on: 6. jun. 2025
 *      Author: theis
 */


/*
 * fixp.c
 *
 *  Created on: 8. jan. 2025
 *      Author: theis
 */


#include <stdint.h> // Whatever needs to be included
#include <stdio.h>
#include "fixp.h"

int32_t squareroot(uint32_t x) {
	/*
	 * Input: a 32bit 16.16 variable.
	 * Returns: The square root of the variable.
	 */

    uint64_t scaled_value = (uint64_t)x << 16;

    uint64_t approx = scaled_value; // Initial guess
    uint64_t better_approx;
    do {
        better_approx = (approx + scaled_value / approx) / 2;
        if (better_approx == approx) break;
        approx = better_approx;
    } while (1);

    return (uint32_t)(approx);
}

int32_t Rational(int a, int b) {
	return(FIX16_DIV(convert(a), convert(b)));
}

void printFix(int32_t i) {
	/*
	 * Input: A signed 16.16 integer to be printed.
	 */

        if ((i & 0x80000000) != 0) { // Handle negative numbers
            printf("-");
            i = ~i + 1;
        }
        printf("%ld.%04ld", i >> 16, (10000 * (uint32_t)(i & 0xFFFF)) >> 16);
        // Print a maximum of 4 decimal digits to avoid overflow
    }

int32_t expand(int32_t i) {
	/*
	 * Input: A number to be bit-shifted by <<2, e.g. 18.14.
	 * Output: Number bitshifted two to the left, e.g. 16.16.
	 */

       return i << 2;
   }

int32_t convert(int32_t i) {
	/*
	 * Input: A number to be bitshifted by <<16, e.g. 32.0.
	 * Output: Number bitshifted 16 to the left, e.g. 16.16.
	 */
	return(i << FIX16_SHIFT);
}

int32_t inconvert(int32_t i) {
	/*
	 * Input: A 16.16 number.
	 * Returns: A 32.0 number, rounded to the nearest integer.
	 */
	return((i + ((1 << (FIX16_SHIFT - 1)))) >> FIX16_SHIFT);
}

int32_t abs(int32_t i) {
	if (i < 0) {
		return -i;
	}
	else {
		return i;
	}
}
