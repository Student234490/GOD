#include <stdio.h> // for printing
#include <fixp.h>
#include <triad.h>
#include <vector.h>
#include <luts.h>
#define PI16 205887


void triad(Vector3D B1, Vector3D g1, Vector3D B2, Vector3D g2, Matrix3x3* result){
	// Normalisér alle vektorerne
	int32_t nB1 = norm(B1);
	int32_t ng1 = norm(g1);
	int32_t nB2 = norm(B2);
	int32_t ng2 = norm(g2);

	B1.x = FIX16_DIV(B1.x, nB1);
	B1.y = FIX16_DIV(B1.y, nB1);
	B1.z = FIX16_DIV(B1.z, nB1);

	g1.x = FIX16_DIV(g1.x, ng1);
	g1.y = FIX16_DIV(g1.y, ng1);
	g1.z = FIX16_DIV(g1.z, ng1);

	B2.x = FIX16_DIV(B2.x, nB2);
	B2.y = FIX16_DIV(B2.y, nB2);
	B2.z = FIX16_DIV(B2.z, nB2);

	g2.x = FIX16_DIV(g2.x, ng2);
	g2.y = FIX16_DIV(g2.y, ng2);
	g2.z = FIX16_DIV(g2.z, ng2);

	Vector3D t1;
	Vector3D t2;
	Vector3D t3;

	Vector3D u1;
	Vector3D u2;
	Vector3D u3;

	t1 = B1;
	u1 = B2;

	Vector3D B1xg1 = cross(B1, g1);

	int32_t nB1g1 = norm(B1xg1);
	t2.x = FIX16_DIV(B1xg1.x, nB1g1);
	t2.y = FIX16_DIV(B1xg1.y, nB1g1);
	t2.z = FIX16_DIV(B1xg1.z, nB1g1);

	t3 = cross(t1, t2);

	Vector3D B2xg2 = cross(B2, g2);

	int32_t nB2g2 = norm(B2xg2);
	u2.x = FIX16_DIV(B2xg2.x, nB2g2);
	u2.y = FIX16_DIV(B2xg2.y, nB2g2);
	u2.z = FIX16_DIV(B2xg2.z, nB2g2);
	u3 = cross(u1, u2);

	Matrix3x3 U = create_matrix_from_vectors(u1, u2, u3);
	Matrix3x3 T = create_matrix_from_vectors(t1, t2, t3);

	Matrix3x3 C;
	Matrix3x3 Tt = transpose(T);

	C = MMult(U,Tt);
	*result = C;
}
//////////////////////////fuld yap skal lave en LUT
int32_t fix_asin(int32_t x) {
    // Clamp x to [-1, 1]
    if (x < -convert(1)) x = -convert(1);
    if (x > convert(1))  x = convert(1);

    int32_t x2 = FIX16_MULT(x, x);
    int32_t x3 = FIX16_MULT(x2, x);
    int32_t term = FIX16_DIV(x3, convert(6.0f)); // x^3 / 6

    return x + term; // Output in radians (fixed-point)
}

int32_t fix_atan2(int32_t y, int32_t x) {
    if (x == 0) {
        return (y > 0) ? PI16 / 2 : -PI16 / 2;
    }

    int32_t abs_y = (y >= 0) ? y : -y;
    int32_t abs_x = (x >= 0) ? x : -x;
    int32_t angle;

    if (abs_y < abs_x) {
        int32_t r = FIX16_DIV(abs_y, abs_x);
        angle = FIX16_MULT(r, PI16 / 4); // ≈ atan(r) for r ∈ [0,1]
    } else {
        int32_t r = FIX16_DIV(abs_x, abs_y);
        angle = PI16 / 2 - FIX16_MULT(r, PI16 / 4); // ≈ atan(r) for r ∈ [1,∞]
    }

    // Adjust to correct quadrant
    if (x > 0 && y >= 0) return angle;
    if (x > 0 && y <  0) return -angle;
    if (x < 0 && y >= 0) return PI16 - angle;
    if (x < 0 && y <  0) return -PI16 + angle;

    return 0;
}
//////////////////////////////////////////////////////////////
#define Q16_SHIFT   16
#define Q16_ONE     (1 << Q16_SHIFT)

/*----- helpers: float ⟷ Q16.16 ----------------------------------------*/
int32_t q16_from_float(float f)
{
    return (int32_t)(f * (float)Q16_ONE + (f >= 0 ? 0.5f : -0.5f));
}
static inline float q16_to_float(int32_t q)
{
    return (float)q / (float)Q16_ONE;
}

/*----- reference asin:  returns radians in Q16.16 ----------------------*/
int32_t asin16(int32_t x_q16)
{
    float xf = q16_to_float(x_q16);          /*  -1 … +1                  */
    float rf = asinf(xf);                    /*  radians                  */
    return q16_from_float(rf);               /*  back to Q16.16           */
}

/*----- reference atan2:  returns radians in Q16.16 ---------------------*/
int32_t atan2_16(int32_t y_q16, int32_t x_q16)
{
    float yf = q16_to_float(y_q16);
    float xf = q16_to_float(x_q16);
    float rf = atan2f(yf, xf);               /*  radians                  */
    return q16_from_float(rf);
}

// In triad.c
void rot2eulerZYX(const Matrix3x3* R, Vector3D* angles) {
    // 1. Correct pitch calculation (remove negation)
    int32_t rzx = R->z.x;

    // 2. Proper clamping
    rzx = (rzx < convert(-1)) ? convert(-1) : (rzx > convert(1)) ? convert(1) : rzx;

    // 3. Use LUT-based asin for accuracy
    int32_t pitch_rad = -asin16(rzx);

    int32_t cos_pitch = cosrad(pitch_rad);
    int32_t rad2deg = FIX16_DIV(convert(180), PI16);
    int32_t pitch_deg = FIX16_MULT(pitch_rad, rad2deg);

    int32_t roll_deg = 0;
    int32_t yaw_deg = 0;

    // 4. Optimized gimbal lock detection
    #define GIMBAL_THRESHOLD 30  // 0.001 in 16.16 (1 << 16)/1000 ≈ 65
    if (abs(cos_pitch) > GIMBAL_THRESHOLD) {
        // 5. Use LUT-based atan2 with corrected arguments
        int32_t roll_rad = atan2_16(R->z.y, R->z.z);  // Standard ZYX formula
        int32_t yaw_rad = atan2_16(R->y.x, R->x.x);   // Standard ZYX formula

        roll_deg = FIX16_MULT(roll_rad, rad2deg);
        yaw_deg = FIX16_MULT(yaw_rad, rad2deg);
    } else {
        // 6. Correct gimbal lock handling
        int32_t yaw_rad = atan2_16(-R->x.y, R->y.y);
        yaw_deg = FIX16_MULT(yaw_rad, rad2deg);
        // Set roll to 0 during gimbal lock (standard approach)
        roll_deg = 0;
    }

    // 7. Efficient angle wrapping
    #define ANGLE_180 (180 << 16)
    #define ANGLE_360 (360 << 16)

    roll_deg %= ANGLE_360;
    roll_deg = (roll_deg > ANGLE_180) ? roll_deg - ANGLE_360 :
              (roll_deg < -ANGLE_180) ? roll_deg + ANGLE_360 : roll_deg;

    yaw_deg %= ANGLE_360;
    yaw_deg = (yaw_deg > ANGLE_180) ? yaw_deg - ANGLE_360 :
             (yaw_deg < -ANGLE_180) ? yaw_deg + ANGLE_360 : yaw_deg;

    angles->x = roll_deg;
    angles->y = pitch_deg;
    angles->z = yaw_deg;

}

