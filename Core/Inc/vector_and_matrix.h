/*
 * vector_and_matrix.h
 *
 *  Created on: 9. jun. 2025
 *      Author: Rasmus
 */

#ifndef INC_VECTOR_AND_MATRIX_H_
#define INC_VECTOR_AND_MATRIX_H_

#include <fixp.h>
#include <stdint.h>
#include <math.h>

////////////////// Macros //////////////////
#define Mult(a, b) FIX16_MULT(a, b)
#define Div(a, b)  FIX16_DIV(a,b)
#define sqrt(a) squareroot(a)

////////////////// Vector //////////////////
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} Vector3D;

// Vector operations
int32_t dot(Vector3D a, Vector3D b);
Vector3D cross(Vector3D a, Vector3D b);
Vector3D add_vector(Vector3D a, Vector3D b);
Vector3D subtract_vector(Vector3D a, Vector3D b);
int32_t norm(Vector3D a);
int32_t cos_theta(Vector3D a, Vector3D b);

////////////////// Matrix //////////////////
typedef struct {
    Vector3D x;
    Vector3D y;
    Vector3D z;
} Matrix3x3;

// Matrix creation
Matrix3x3 create_matrix(int32_t a, int32_t b, int32_t c,
                         int32_t d, int32_t e, int32_t f,
                         int32_t g, int32_t h, int32_t i);
Matrix3x3 create_matrix_from_vectors(Vector3D a, Vector3D b, Vector3D c);

// Matrix operations
Matrix3x3 transpose(Matrix3x3 a);
Matrix3x3 MMult(Matrix3x3 a, Matrix3x3 b);

// Utility
void print_matrix(Matrix3x3 a);



#endif /* INC_VECTOR_AND_MATRIX_H_ */
