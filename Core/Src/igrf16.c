/*
 * igrf16.c
 *
 *  Created on: 18. jun. 2025
 *      Author: theis
 */

#include "igrf16.h"
#include "fixp.h"
#include "luts.h"
#include "math.h"
#include <stdio.h>
#define FIXED_POINT_SCALE (1 << 16)

#define wgs84_f 220;
#define wgs84_e2 439;
#define wgs84_a 417997586 // 6378.137;
#define wgs84_b 416596119 // wgs84_a * (1 - wgs84_f), wgs84_f = 220 # 1 / 298.257223563;


// Convert 16.16 fixed-point to float
static inline float fixed_to_float(int32_t x) {
    return (float)x / FIXED_POINT_SCALE;
}

// Convert float to 16.16 fixed-point
static inline int32_t float_to_fixed(float x) {
    return (int32_t)(x * FIXED_POINT_SCALE);
}

#define MULT(a,b) FIX16_MULT((a),(b))
#define POW(a,b)  FIX16_POW((a),(b))
#define DIV(a,b)  FIX16_DIV((a),(b))

// Decimal years since January 1, IGRF_START_YEAR
int32_t get_years16(const igrf_time_t dt)
{
  if ((dt.year < IGRF_START_YEAR) || (dt.year >= IGRF_END_YEAR) ||
      (dt.month < 1) || (dt.month > 12) ||
      (dt.day < 1) || (dt.hour > 23) ||
      (dt.minute > 59) || (dt.second > 59))
  {
    return -1;
  }

  int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int is_leap = (((dt.year % 4) == 0) && (((dt.year % 100) != 0) || ((dt.year % 400) == 0)));

  // Adjust for leap year
  if (is_leap)
  {
    days_in_month[1] = 29;
  }

  // Check valid day in the month
  if (dt.day > days_in_month[dt.month - 1])
  {
    return -1;
  }

  // Days since IGRF_START_YEAR
  int years = dt.year - IGRF_START_YEAR;
  int32_t days_arr[] = {0, 31, 59, 90, 120, 151, 182, 212, 243, 273, 304, 334};
  int32_t days = days_arr[dt.month - 1] + dt.day + (dt.month > 2 ? is_leap : 0);
  int32_t hours = (dt.hour << 16) + DIV(dt.minute, 60 << 16) + DIV(dt.second, 3600 << 16);
  int32_t total_days = is_leap ? 366 : 365;

  // Decimal years
  return (years << 16) + DIV(days << 16,total_days << 16) + DIV(hours,24 << 16);
}

/**
 * @brief Computes magnetic field strength [nT] in specified coordinate frame.
 *
 * @param t Time between January 1, IGRF_START_YEAR and December 31, IGRF_END_YEAR.
 * @param longitude Latitude [deg] Q16.16.
 * @param latitude Latitude [deg] Q16.16.
 * @param altitude Altitude [km] Q16.16.
 *          - For geodetic frame, altitude represents height above Earth's surface.
 *          - For geocentric frame, altitude represents radius from the center of the Earth.
 * @param f Frame type - geodetic or geocentric frame.
 * @param b Output magnetic field intensity in specified frame.
 * @param f The frame of reference for the input coordinates and the output field values.
 *
 * @return false if the time is out of range; true otherwise.
 */
int igrf16(const igrf_time_t t, const int32_t latitude, const int32_t longitude, const int32_t altitude, igrf_frame_t f, int32_t b[3])
{
  const int32_t a = 417542963; // 6371.2;                  // Radius of Earth [km]
  const int32_t theta = M16_PI_2 - MULT(latitude, D162R); // Colattitude [rad]
  const int32_t phi = MULT(longitude, D162R);            // Longitude [rad]

  int32_t ct = cosrad(theta);
  int32_t st = sinrad(theta);
  int32_t cd, sd, r;

  // Geodetic to geocentric conversion
  // https://github.com/wb-bgs/m_IGRF
  if (f == IGRF_GEODETIC)
  {
    // Radius
    const int32_t h = altitude;
    const int32_t rho = hypot(MULT(wgs84_a, st), MULT(wgs84_b, ct));
    r = squareroot64(h * h + 2 * h * rho + (pow(wgs84_a, 4) * st * st + pow(wgs84_b, 4) * ct * ct) / (rho * rho));

    // Latitude
    cd = DIV((h + rho), r);
    sd = (wgs84_a * wgs84_a - wgs84_b * wgs84_b) / rho * ct * st / r;
    const double temp = ct;
    ct = cd * ct - sd * st;
    st = cd * st + sd * temp;
  }
  else {
	 r = altitude;
  }


  // Avoid singularity on pole
  const int32_t epsilon = 1;

  if (st < epsilon && st > -epsilon)
  {
    st = epsilon;
  }

  int32_t years = get_years16(t);

  if (years < 0)
  {
    return 0;
  }

  // [a] Re-occurring power factors
  // Optimizations [a] and [b] by Alar Leibak.
  int32_t ar_pow[IGRF_DEGREE + 1];
  const int32_t ar = DIV(a, r);
  ar_pow[0] = MULT(MULT(ar, ar), ar);

  for (uint8_t i = 1; i <= IGRF_DEGREE; i++)
  {
    ar_pow[i] = MULT(ar_pow[i - 1], ar);
  }

  // [b] Re-occurring sines and cosines
  int32_t sines[IGRF_DEGREE + 1], cosines[IGRF_DEGREE + 1];
  sines[0] = 0;
  cosines[0] = 1 << 16;
  sines[1] = sinrad(phi);
  cosines[1] = cosrad(phi);

  for (uint8_t i = 2; i <= IGRF_DEGREE; i++)
  {
    if (i & 1)
    {
      sines[i] = MULT(sines[i - 1], cosines[1]) + MULT(cosines[i - 1], sines[1]);
      cosines[i] = MULT(cosines[i - 1], cosines[1]) - MULT(sines[i - 1], sines[1]);
    }
    else // even
    {
      sines[i] = MULT(sines[i >> 1] << 1, cosines[i >> 1]);
      cosines[i] = MULT(cosines[i >> 1] << 1, cosines[i >> 1]) - convert(1);
    }
  }

  // Associated Legendre polynomials and its derivative
  int32_t pnm = 0, dpnm = 0; // (n, m)
  int32_t p11 = convert(1), dp11 = 0; // (n, n)
  int32_t p10 = convert(1), dp10 = 0; // (n-1, m)
  int32_t p20 = 0, dp20 = 0; // (n-2, m)

  // Field components: radial, theta, and phi
  int32_t br = 0, bt = 0, bp = 0;

  for (uint8_t m = 0; m <= IGRF_DEGREE; m++)
  {
    for (uint8_t n = 1; n <= IGRF_DEGREE; n++)
    {
      if (m <= n)
      {
        if (n == m)
        {
          pnm = MULT(st, p11);
          dpnm = MULT(st, dp11) + MULT(ct, p11);

          p11 = pnm; dp11 = dpnm;
          p20 = 0; dp20 = 0;
        }
        else
        {
          int32_t Knm = 0;

          if (n > 1)
          {
            Knm = DIV(
            		POW(convert(n - 1), convert(2)) - POW(convert(m), convert(2)),
					MULT((MULT(convert(n), convert(2)) - convert(1)), MULT(convert(2), convert(n)) - convert(3))
					);
          }

          pnm = MULT(ct, p10) - MULT(Knm,p20);
          dpnm = MULT(ct, dp10) - MULT(st, p10) - MULT(Knm, dp20);
        }

        p20 = p10; dp20 = dp10;
        p10 = pnm; dp10 = dpnm;

        // Linear interpolation of g and h
        const int k = ((n * (n + 1)) >> 1) + m - 1;
        const int32_t g = (g_val16[k] + MULT(g_sv16[k],years)) >> 2;
        const int32_t h = (h_val16[k] + MULT(h_sv16[k],years)) >> 2;

        if (m == 0)
        {
          const int32_t temp = MULT(ar_pow[n - 1], g);
          br += MULT(MULT(temp, convert(n + 1)), pnm);
          bt -= MULT(temp,dpnm);
        }
        else
        {
          const int32_t hsin = MULT(h, sines[m]);
          const int32_t hcos = MULT(h, cosines[m]);
          const int32_t gsin = MULT(g, sines[m]);
          const int32_t gcos = MULT(g, cosines[m]);
          const int32_t temp = MULT(ar_pow[n - 1], gcos + hsin);

          br += MULT(temp, MULT(convert(n + 1), pnm));
          bt -= MULT(temp, dpnm);
          bp -= MULT(ar_pow[n - 1], MULT(convert(m), MULT((-gsin + hcos), pnm)));
        }
        printf("%i,%i,%i,%f,%f,%f,%f,%f,%f,%f\r\n", n, m, k, fixed_to_float(g), fixed_to_float(h), fixed_to_float(pnm),fixed_to_float(dpnm), fixed_to_float(br), fixed_to_float(bt), fixed_to_float(bp));
      }
    }
  }

  bp = DIV(bp, st);

  // Geocentric NED
  b[0] = -bt;
  b[1] = bp;
  b[2] = -br;

  // Geocentric to geodetic NED
  if (f == IGRF_GEODETIC)
  {
    int32_t temp = b[0];
    b[0] = MULT(cd, b[0]) + MULT(sd, b[2]);
    b[2] = MULT(cd, b[2]) - MULT(sd, temp);
  }

  return 1;
}
