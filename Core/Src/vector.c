#include <stdint.h>
#include <math.h>
#include <vector.h>
#include <fixp.h>
#include "luts.h"

//////////////////Vector////////////////////////////////////////////

int32_t dot(Vector3D a, Vector3D b) {
    return Mult(a.x, b.x) + Mult(a.y, b.y) + Mult(a.z, b.z);
}

Vector3D cross(Vector3D a, Vector3D b){
	Vector3D c;
	c.x = Mult(a.y,b.z) - Mult(a.z,b.y);
	c.y = Mult(a.z,b.x) - Mult(a.x,b.z);
	c.z = Mult(a.x,b.y) - Mult(a.y,b.x);
	return c;
}

Vector3D add_vector(Vector3D a, Vector3D b){
	Vector3D vector;
	vector.x = a.x + b.x;
	vector.y = a.y + b.y;
	vector.z = a.z + b.z;
	return vector;
}

Vector3D scale_vector(Vector3D a, int32_t k) {
	Vector3D vector;
	vector.x = Mult(a.x,k);
	vector.y = Mult(a.y,k);
	vector.z = Mult(a.z,k);
	return vector;
}

Vector3D VMult(Vector3D a, int32_t b){
	Vector3D c;
	c.x = Mult(a.x, b);
	c.y = Mult(a.y, b);
	c.z = Mult(a.z, b);
	return c;
}

Vector3D subtract_vector(Vector3D a, Vector3D b){
	return add_vector(a, scale_vector(b, convert(-1)));
}

int32_t norm(Vector3D a) {
    int64_t sum = (int64_t)Mult(a.x, a.x)
                + (int64_t)Mult(a.y, a.y)
                + (int64_t)Mult(a.z, a.z);
    return sqrt((int32_t)(sum >> 16));
}

int32_t cos_theta(Vector3D a, Vector3D b){ //cos_theta (later use??)
	return Div(dot(a,b), Mult(norm(a),norm(b)));
}

void printFixVector(Vector3D a) {
	printf("[ ");
	printFix(a.x);
	printf(", ");
	printFix(a.y);
	printf(", ");
	printFix(a.z);
	printf(" ]");
}

void printVector(Vector3D a) {
	printf("[ %i, %i, %i ]", (int)a.x, (int)a.y, (int)a.z);
}

//////////////////Matrix////////////////////////////////////////////

Matrix3x3 create_matrix(int32_t a, int32_t b, int32_t c,
                        int32_t d, int32_t e, int32_t f,
                        int32_t g, int32_t h, int32_t i) {
    Matrix3x3 matrix;
    matrix.x.x = a; matrix.x.y = d; matrix.x.z = g;
    matrix.y.x = b; matrix.y.y = e; matrix.y.z = h;
    matrix.z.x = c; matrix.z.y = f; matrix.z.z = i;
    return matrix;
}

Matrix3x3 create_matrix_from_vectors(Vector3D a, Vector3D b, Vector3D c) {
    Matrix3x3 matrix;
    matrix.x = a;
    matrix.y = b;
    matrix.z = c;
    return matrix;
}

Matrix3x3 transpose(Matrix3x3 a){
	Vector3D a_vect1;
	Vector3D a_vect2;
	Vector3D a_vect3;

	a_vect1.x = a.x.x;
	a_vect1.y = a.y.x;
	a_vect1.z = a.z.x;

	a_vect2.x = a.x.y;
	a_vect2.y = a.y.y;
	a_vect2.z = a.z.y;

	a_vect3.x = a.x.z;
	a_vect3.y = a.y.z;
	a_vect3.z = a.z.z;

	Matrix3x3 b; // b er brugt til at gemme data
	b.x = a_vect1;
	b.y = a_vect2;
	b.z = a_vect3;

	return b;
}

Matrix3x3 MMult(Matrix3x3 a, Matrix3x3 b){ //Matrix mult
	Vector3D b_vect1;
	Vector3D b_vect2;
	Vector3D b_vect3;
	b_vect1 = b.x;
	b_vect2 = b.y;
	b_vect3 = b.z;

	////a////
	a = transpose(a);
	Vector3D a_vect1;
	Vector3D a_vect2;
	Vector3D a_vect3;
	a_vect1 = a.x;
	a_vect2 = a.y;
	a_vect3 = a.z;

	//////calc
	Matrix3x3 svar;
	svar.x.x = dot(a_vect1, b_vect1);
	svar.y.x = dot(a_vect1, b_vect2);
	svar.z.x = dot(a_vect1, b_vect3);

	svar.x.y = dot(a_vect2, b_vect1);
	svar.y.y = dot(a_vect2, b_vect2);
	svar.z.y = dot(a_vect2, b_vect3);

	svar.x.z = dot(a_vect3, b_vect1);
	svar.y.z = dot(a_vect3, b_vect2);
	svar.z.z = dot(a_vect3, b_vect3);

	return svar;
}

Vector3D MVMult(Matrix3x3 a, Vector3D b){
	a = transpose(a);
	Vector3D a_vect1;
	Vector3D a_vect2;
	Vector3D a_vect3;
	a_vect1 = a.x;
	a_vect2 = a.y;
	a_vect3 = a.z;

	Vector3D c;
	c.x = dot(a_vect1, b);
	c.y = dot(a_vect2, b);
	c.z = dot(a_vect3, b);
	return c;
}

void printFixMatrix(Matrix3x3 a) {
	printf("\r\n[ ");
	printFix(a.x.x);
	printf(", ");
	printFix(a.y.x);
	printf(", ");
	printFix(a.z.x);
	printf(" ] \r\n");

	printf("[ ");
	printFix(a.x.y);
	printf(", ");
	printFix(a.y.y);
	printf(", ");
	printFix(a.z.y);
	printf(" ] \r\n");

	printf("[ ");
	printFix(a.x.z);
	printf(", ");
	printFix(a.y.z);
	printf(", ");
	printFix(a.z.z);
	printf(" ] \r\n");

}

void print_matrix(Matrix3x3 a) {
    printf("(%ld, %ld, %ld)\n", a.x.x, a.y.x, a.z.x);
    printf("(%ld, %ld, %ld)\n", a.x.y, a.y.y, a.z.y);
    printf("(%ld, %ld, %ld)\n", a.x.z, a.y.z, a.z.z);
}

// raw square in Q32.32
static inline int64_t SquareRaw(int32_t A) {
    return (int64_t)A * (int64_t)A;
}

// integer sqrt of a 64bit value
static inline uint32_t isqrt_u64(uint64_t x) {
    uint64_t op = x;
    uint64_t res = 0;
    // highest power-of-4 <= 2^62
    uint64_t one = (uint64_t)1 << 62;
    while (one > op) one >>= 2;
    while (one) {
        if (op >= res + one) {
            op  -= res + one;
            res  = (res >> 1) + one;
        } else {
            res >>= 1;
        }
        one >>= 2;
    }
    return (uint32_t)res;
}

// Q16.16 vector norm
int32_t norm_q16(Vector3D a) {
    uint64_t sum = 0;
    sum += (uint64_t)SquareRaw(a.x);
    sum += (uint64_t)SquareRaw(a.y);
    sum += (uint64_t)SquareRaw(a.z);
    // sum // Q32.32
    // sqrt(sum) gives Q16.16 directly
    return (int32_t)isqrt_u64(sum);
}

static const int32_t DEG2RAD_Q16 = 1144; //pi/180 in Q16.16


Vector3D rotateZ14(Vector3D *v, int32_t yaw, int32_t pitch, int32_t roll)
{
    /* sin / cos in Q16.16 */
    int32_t cyaw   = cosrad(yaw),   syaw   = sinrad(yaw);
    int32_t cpitch = cosrad(pitch), spitch = sinrad(pitch);
    int32_t croll  = cosrad(roll),  sroll  = sinrad(roll);

    /* Rx(roll) ------------------------------------------------------- */
    int32_t y1 = FIX16_MULT(v->y, croll) - FIX16_MULT(v->z, sroll);
    int32_t z1 = FIX16_MULT(v->y, sroll) + FIX16_MULT(v->z, croll);
    int32_t x1 = v->x;

    /* Ry(pitch) ------------------------------------------------------ */
    int32_t x2 = FIX16_MULT(x1, cpitch) + FIX16_MULT(z1, spitch);
    int32_t z2 = FIX16_MULT(-x1, spitch) + FIX16_MULT(z1, cpitch);
    int32_t y2 = y1;

    /* Rz(yaw) -------------------------------------------------------- */
    int32_t x3 = FIX16_MULT(x2, cyaw) - FIX16_MULT(y2, syaw);
    int32_t y3 = FIX16_MULT(x2, syaw) + FIX16_MULT(y2, cyaw);

    Vector3D out = { x3, y3, z2 };
    return out;
}

void rotate_ref_vectors(Vector3D *g, Vector3D *m,
                        int32_t yaw_deg, int32_t pitch_deg, int32_t roll_deg)
{
    int32_t yaw   = FIX16_MULT(yaw_deg,   DEG2RAD_Q16);
    int32_t pitch = FIX16_MULT(pitch_deg, DEG2RAD_Q16);
    int32_t roll  = FIX16_MULT(roll_deg,  DEG2RAD_Q16);

    *g = rotateZ14(g, yaw, pitch, roll);
    *m = rotateZ14(m, yaw, pitch, roll);
}

