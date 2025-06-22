/*
 * igrf16.h
 *
 *  Created on: 18. jun. 2025
 *      Author: theis
 */

#ifndef INC_IGRF16_H_
#define INC_IGRF16_H_

#include "igrf16_2025.h"
#include <stdint.h>

#define R162D 3754936
#define D162R 1144
#define M16_PI_2 102944

typedef struct
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} igrf_time_t;


typedef enum
{
  IGRF_GEODETIC,
  IGRF_GEOCENTRIC
} igrf_frame_t;

int igrf16(const igrf_time_t t, const int32_t latitude, const int32_t longitude, const int32_t altitude, igrf_frame_t f, int32_t b[3]);

#endif /* INC_IGRF16_H_ */
