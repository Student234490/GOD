/*
 * magnet.h
 *
 *  Created on: 11. jun. 2025
 *      Author: Rasmus
 */
#ifndef MAGNET_H
#define MAGNET_H
#include <stdint.h>

int32_t normalizeTheta(int32_t theta_q16);
int32_t toRad(int32_t deg_q16);
void    magnet(int32_t r_q16, int32_t theta_q16, int32_t phi_q16,
               int32_t days_q16, int32_t out_vec[3]);

#endif
